#include "ppu.h"
#include "mapper.h"
#include "machine.h"
#include "cpu.h"
#include <qmath.h>
#include <QDataStream>

static const u8 paletteDefaultMem[] = {
	0,	1,	2,	3,
	0,	5,	6,	7,
	0,	9,	10,	11,
	0,	13,	14,	15,
	0,	17,	18,	19,
	0,	21,	22,	23,
	0,	25,	26,	27,
	0,	29,	30,	31
};

QImage nesPpuFrame;
NesPpu nesPpu;

static NesPpu::ChipType ppuType;
static NesPpu::RenderMethod ppuRenderMethod;

u8 nesPpuRegs[4]; // registers at 0x2000-0x2003
static bool regToggle;
static u8 dataLatch;
static u16 incrementValue;
static uint spriteSize;
static u8 bufferedData;
static u8 securityValue;

int nesPpuScanline;
int nesPpuScanlinesPerFrame;
static QRgb *scanlineData;

static bool characterLatchEnabled = false;
static bool externalLatchEnabled = false;

bool vBlankOut;

static u16 nesVramAddress;
static u16 refreshLatch;
static u8 scrollTileXOffset;
u8 nesPpuScrollTileYOffset;
u16 nesPpuTilePageOffset;
static u16 spritePageOffset;
static u16 loopyShift;
static u8 bgWritten[33];
static u8 bit2Rev[256];

static const int NumSprites = 64;
static u8 spriteMem[NumSprites*4];

static u8 paletteMem[32];
static QRgb palettePens[512];
static QRgb palettePenLut[32];
static u32 paletteMask;
static u32 paletteEmphasis;
static bool palettePenLutNeedsRebuild;

static void fillPens();

static void buildPenLUT() {
	for (int i = 0; i < 32; i++)
		palettePenLut[i] = palettePens[(paletteMem[i] & paletteMask) + paletteEmphasis];
}

static void updateColorEmphasisAndMask() {
	u32 newEmphasis = u32(nesPpuRegs[NesPpu::Control1] & NesPpu::BackgroundColorCR1Bit);
	if (newEmphasis != 0x20 && newEmphasis != 0x40 && newEmphasis != 0x80)
		newEmphasis = 0x00;
	newEmphasis *= 2;
	u32 newMask = ((nesPpuRegs[NesPpu::Control1] & NesPpu::MonochromeModeCR1Bit) ? 0xF0 : 0xFF);
	if (newEmphasis != paletteEmphasis || newMask != paletteMask) {
		paletteEmphasis = newEmphasis;
		paletteMask = newMask;
		palettePenLutNeedsRebuild = true;
	}
}

void NesPpu::init() {
	nesPpuFrame = QImage(8+VisibleScreenWidth+8, VisibleScreenHeight, QImage::Format_RGB32);

	setChipType(PPU2C02);
	setRenderMethod(PreRender);

	nesPpuScanline = 0;
	scanlineData = 0;

	characterLatchEnabled = false;
	externalLatchEnabled = false;

	vBlankOut = false;

	nesVramAddress = 0;
	refreshLatch = 0;
	scrollTileXOffset = 0;
	nesPpuScrollTileYOffset = 0;
	nesPpuTilePageOffset = 0;
	spritePageOffset = 0;
	loopyShift = 0;

	for (int i = 0; i < 256; i++) {
		u8 m = 0x80;
		u8 c = 0;
		for (int j = 0; j < 8; j++) {
			if (i & (1<<j))
				c |= m;
			m >>= 1;
		}
		bit2Rev[i] = c;
	}

	qMemSet(spriteMem, 0, sizeof(spriteMem));

	qMemSet(nesPpuRegs, 0, sizeof(nesPpuRegs));
	regToggle = false;
	dataLatch = 0;
	incrementValue = 1;
	spriteSize = 8;
	bufferedData = 0;
	securityValue = 0x00;

	fillPens();
	qMemCopy(paletteMem, paletteDefaultMem, sizeof(paletteMem));
	updateColorEmphasisAndMask();
}

static void writePalette(u16 address, u8 data) {
	Q_ASSERT(address < 32);
	data &= 0x3F;
	if (!(address & 0x03)) {
		if (!(address & 0x0F)) {
			if (paletteMem[0] != data) {
				for (int i = 0; i < 32; i += 4)
					paletteMem[i] = data;
				palettePenLutNeedsRebuild = true;
			}
		}
	} else {
		if (paletteMem[address] != data) {
			paletteMem[address] = data;
			palettePenLutNeedsRebuild = true;
		}
	}
}

static u8 readPalette(u16 address) {
	Q_ASSERT(address < 32);
	return paletteMem[address] & paletteMask;
}

static QRgb *currentPens() {
	if (palettePenLutNeedsRebuild) {
		buildPenLUT();
		palettePenLutNeedsRebuild = false;
	}
	return palettePenLut;
}

static void updateVBlankOut() {
	bool vBlankEnabled = (nesPpuRegs[NesPpu::Control0] & NesPpu::VBlankEnableCR0Bit);
	bool vBlankState = (nesPpuRegs[NesPpu::Status] & NesPpu::VBlankSRBit);
	bool newVBlank = (vBlankEnabled && vBlankState);
	if (newVBlank != vBlankOut) {
		vBlankOut = newVBlank;
		nesCpu.nmi_i(vBlankOut);
	}
}

void NesPpu::writeReg(u16 address, u8 data) {
	Q_ASSERT(address < 8);
	if (securityValue && !(address & 6))
		address ^= 1;
	switch (static_cast<Register>(address)) {
	case Control0:
		nesPpuRegs[Control0] = data;
		updateVBlankOut();
		/* update the name table number on our refresh latches */
		refreshLatch &= ~(3 << 10);
		refreshLatch |= (data & 3) << 10;
		/* the char ram bank points either 0x0000 or 0x1000 (page 0 or page 4) */
		nesPpuTilePageOffset = (data & BackgroundTableCR0Bit) << 8;
		spritePageOffset = (data & SpriteTableCR0Bit) << 9;
		incrementValue = ((data & IncrementCR0Bit) ? 32 : 1);
		spriteSize = ((nesPpuRegs[Control0] & SpriteSizeCR0Bit) ? 16 : 8);
		break;
	case Control1:
		nesPpuRegs[Control1] = data;
		updateColorEmphasisAndMask();
		break;
	case SpriteRAMAddress:
		nesPpuRegs[SpriteRAMAddress] = data;
		break;
	case SpriteRAMIO:
		/* if the PPU is currently rendering the screen,
		 * 0xff is written instead of the desired data. */
		if (nesPpuScanline < NesPpu::VisibleScreenHeight)
			data = 0xFF;
		spriteMem[nesPpuRegs[SpriteRAMAddress]++] = data;
		break;
	case Scroll:
		if (regToggle) {
			refreshLatch &= ~((0xF8 << 2) | (7 << 12));
			refreshLatch |= (data & 0xF8) << 2;
			refreshLatch |= (data & 7) << 12;
		} else {
			refreshLatch &= ~0x1F;
			refreshLatch |= (data >> 3) & 0x1F;
			scrollTileXOffset = data & 7;
			// TODO check if it works with loopy_shift = ppu->scrollTileXOffset if yes remove
		}
		regToggle = !regToggle;
		break;
	case VRAMAddress:
		if (regToggle) {
			refreshLatch &= 0xFF00;
			refreshLatch |= data;
			nesVramAddress = refreshLatch;
			nesMapper->addressBusLatch(nesVramAddress);
		} else {
			refreshLatch &= 0x00FF;
			refreshLatch |= (data & 0x3F) << 8;
		}
		regToggle = !regToggle;
		break;
	case VRAMIO: {
		u16 vramAddress = nesVramAddress & 0x3FFF;
		if (vramAddress >= NesPpu::PalettesAddress)
			writePalette(vramAddress & 0x1F, data);
		else if (nesMapper->ppuBank1KType(vramAddress >> 10) != VromBank)
			nesMapper->ppuWrite(vramAddress, data);
		nesVramAddress += incrementValue;
		break;
	}
	default:
		break;
	}
	dataLatch = data;
}

u8 NesPpu::readReg(u16 address) {
	Q_ASSERT(address < 8);
	switch (static_cast<Register>(address)) {
	case Status:
		/* the top 3 bits of the status register are the only ones that report data. The
		   remainder contain whatever was last in the PPU data latch, except on the RC2C05 (protection) */
		if (securityValue) {
			dataLatch = nesPpuRegs[Status] & (VBlankSRBit | Sprite0HitSRBit);
			dataLatch |= securityValue;
		} else {
			dataLatch = nesPpuRegs[Status] | (dataLatch & 0x1F);
		}
		/* reset hi/lo scroll toggle */
		regToggle = false;
		/* if the vblank bit is set, clear all status bits but the 2 sprite flags */
		if (dataLatch & VBlankSRBit) {
			nesPpuRegs[Status] &= (Sprite0HitSRBit | SpriteMaxSRBit); // TODO [virtuanes ~VBlankSRBit]
			updateVBlankOut();
		}
		break;
	case SpriteRAMIO:
		dataLatch = spriteMem[nesPpuRegs[SpriteRAMAddress]++];
		break;
	case VRAMIO: {
		u16 vramAddress = nesVramAddress & 0x3FFF;
		nesVramAddress += incrementValue;
		if (vramAddress >= NesPpu::PalettesAddress)
			return readPalette(vramAddress & 0x1F);
		else
			dataLatch = bufferedData;
		bufferedData = nesMapper->ppuRead(vramAddress);
		break;
	}
	default:
		break;
	}
	return dataLatch;
}

void NesPpu::setVBlank(bool on) {
	if (on)
		nesPpuRegs[Status] |= VBlankSRBit;
	else
		nesPpuRegs[Status] &= ~(VBlankSRBit | Sprite0HitSRBit);
	updateVBlankOut();
}

static inline void setSpriteMax(bool on) {
	if (on)
		nesPpuRegs[NesPpu::Status] |= NesPpu::SpriteMaxSRBit;
	else
		nesPpuRegs[NesPpu::Status] &= ~NesPpu::SpriteMaxSRBit;
}

void NesPpu::setChipType(ChipType newType) {
	ppuType = newType;
	if (ppuType == PPU2C07)
		nesPpuScanlinesPerFrame = ScanlinesPerFramePAL;
	else
		nesPpuScanlinesPerFrame = ScanlinesPerFrameNTSC;

	switch (ppuType) {
	case NesPpu::PPU2C05_01:	securityValue = 0x1B; break;
	case NesPpu::PPU2C05_02:	securityValue = 0x3D; break;
	case NesPpu::PPU2C05_03:	securityValue = 0x1C; break;
	case NesPpu::PPU2C05_04:	securityValue = 0x1B; break;
	default:					securityValue = 0x00; break;
	}
}


NesPpu::RenderMethod NesPpu::renderMethod() const
{ return ppuRenderMethod; }

void NesPpu::setRenderMethod(RenderMethod method) {
	if (ppuRenderMethod != method) {
		ppuRenderMethod = method;
		emit renderMethodChanged();
	}
}

void NesPpu::nextScanline() {
	nesPpuScanline++;
	scanlineData += nesPpuFrame.bytesPerLine()/sizeof(QRgb);
}

void NesPpu::dma(u8 page) {
	u16 address = page << 8;
	for (int i = 0; i < 256; i++)
		writeReg(SpriteRAMIO, nesMapper->read(address + i));
}

void NesPpu::setCharacterLatchEnabled(bool on)
{ characterLatchEnabled = on; }
void NesPpu::setExternalLatchEnabled(bool on)
{ externalLatchEnabled = on; }

void NesPpu::processFrameStart() {
	nesPpuScanline = 0;
	scanlineData = (QRgb *)nesPpuFrame.bits();
	if (isDisplayOn()) {
		nesVramAddress = refreshLatch;
		loopyShift = scrollTileXOffset;
		nesPpuScrollTileYOffset = (nesVramAddress & 0x7000) >> 12;
	}
}

void NesPpu::processScanlineStart() {
	if (isDisplayOn()) {
		nesVramAddress = (nesVramAddress & 0xFBE0) | (refreshLatch & 0x041F);
		loopyShift = scrollTileXOffset;
		nesPpuScrollTileYOffset = (nesVramAddress & 0x7000) >> 12;
		nesMapper->addressBusLatch(NameTableOffset | (nesVramAddress & 0x0FFF));
	}
}

void NesPpu::processScanlineNext() {
	if (isDisplayOn()) {
		if ((nesVramAddress & 0x7000) == 0x7000) {
			nesVramAddress &= 0x8FFF;
			if ((nesVramAddress & 0x03E0) == 0x03A0) {
				nesVramAddress ^= 0x0800;
				nesVramAddress &= 0xFC1F;
			} else {
				if ((nesVramAddress & 0x03E0) == 0x03E0) {
					nesVramAddress &= 0xFC1F;
				} else {
					nesVramAddress += 0x0020;
				}
			}
		} else {
			nesVramAddress += 0x1000;
		}
		nesPpuScrollTileYOffset = (nesVramAddress & 0x7000) >> 12;
	}
}

void NesPpu::processScanline() {
	drawBackground();
	drawSprites();
}

void NesPpu::drawBackground() {
	qMemSet(bgWritten, 0, sizeof(bgWritten));
	if (!isBackgroundVisible()) {
		fillScanline(0, 8+VisibleScreenWidth);
		if (renderMethod() == TileRender)
			nesMachine.clockCpu(FetchCycles*4*32);
		return;
	}
	if (renderMethod() != TileRender) {
		if (!externalLatchEnabled)
			drawBackgroundNoTileNoExtLatch();
		else
			drawBackgroundNoTileExtLatch();
	} else {
		if (!externalLatchEnabled)
			drawBackgroundTileNoExtLatch();
		else
			drawBackgroundTileExtLatch();
	}
	/* if the left 8 pixels for the background are off, blank them */
	if (!(nesPpuRegs[Control1] & BackgroundClipDisableCR1Bit))
		fillScanline(0, 16);
}

void NesPpu::drawBackgroundNoTileNoExtLatch() {
	QRgb *dst = scanlineData + (8 - loopyShift);

	u16 nameTableAddress = NameTableOffset | (nesVramAddress & 0x0FFF);
	u16 attributeAddress = AttributeTableOffset | ((nameTableAddress&0x0380)>>4);
	u8 nameTableX = nameTableAddress & 0x001F;
	u8 attributeShift = (nameTableAddress & 0x0040) >> 4;
	u8 *nameTable = nesMapper->ppuBank1KData(nameTableAddress >> 10);

	u32 cacheTile = 0xFFFF0000;
	u8 cacheAttribute = 0xFF;

	attributeAddress &= 0x3FF;

	u8 *bgWr = bgWritten;
	QRgb *currPens = currentPens();
	for (int i = 0; i < 33; i++) {
		u16 tileAddress = nameTable[nameTableAddress & 0x03FF] * 0x10;
		tileAddress += nesPpuTilePageOffset + nesPpuScrollTileYOffset;
		u8 attribute = nameTable[attributeAddress + (nameTableX >> 2)];
		attribute >>= (nameTableX & 2) | attributeShift;
		attribute = (attribute & 3) << 2;

		if (cacheTile == tileAddress && cacheAttribute == attribute) {
			// FIXME
//			qMemCopy(dst, dst-8, 8*sizeof(QRgb));
			memcpy(dst, dst-8, 8*sizeof(QRgb));
			*bgWr = *(bgWr - 1);
		} else {
			cacheTile = tileAddress;
			cacheAttribute = attribute;
			u8 plane1 = nesMapper->ppuRead(tileAddress + 0);
			u8 plane2 = nesMapper->ppuRead(tileAddress + 8);
			*bgWr = plane1 | plane2;

			QRgb *pens = currPens + attribute;
			register int c1 = ((plane1>>1)&0x55) | (plane2&0xAA);
			register int c2 = (plane1&0x55) | ((plane2<<1)&0xAA);
			dst[0] = pens[(c1>>6)];
			dst[4] = pens[(c1>>2)&3];
			dst[1] = pens[(c2>>6)];
			dst[5] = pens[(c2>>2)&3];
			dst[2] = pens[(c1>>4)&3];
			dst[6] = pens[c1&3];
			dst[3] = pens[(c2>>4)&3];
			dst[7] = pens[c2&3];
		}
		dst += 8;
		bgWr++;

		/* character latch (for MMC2/MMC4) */
		if (characterLatchEnabled)
			nesMapper->characterLatch(tileAddress);

		if (++nameTableX == 32) {
			nameTableX = 0;
			nameTableAddress ^= 0x41F;
			attributeAddress = AttributeTableOffset | ((nameTableAddress&0x0380)>>4);
			nameTable = nesMapper->ppuBank1KData(nameTableAddress >> 10);
		} else {
			nameTableAddress++;
		}
	}
}

void NesPpu::drawBackgroundTileNoExtLatch() {
	QRgb *dst = scanlineData + (8 - loopyShift);

	u16 nameTableAddress = NameTableOffset | (nesVramAddress & 0x0FFF);
	u16 attributeAddress = AttributeTableOffset | ((nameTableAddress&0x0380)>>4);
	u8 nameTableX = nameTableAddress & 0x001F;
	u8 attributeShift = (nameTableAddress & 0x0040) >> 4;
	u8 *nameTable = nesMapper->ppuBank1KData(nameTableAddress >> 10);

	u32 cacheTile = 0xFFFF0000;
	u8 cacheAttribute = 0xFF;

	attributeAddress &= 0x3FF;

	u8 *bgWr = bgWritten;
	QRgb *currPens = currentPens();
	for (int i = 0; i < 33; i++) {
		u16 tileAddress = nameTable[nameTableAddress & 0x03FF] * 0x10;
		tileAddress += nesPpuTilePageOffset + nesPpuScrollTileYOffset;
		if (i)
			nesMachine.clockCpu(FetchCycles*4);
		u8 attribute = nameTable[attributeAddress + (nameTableX >> 2)];
		attribute >>= (nameTableX & 2) | attributeShift;
		attribute = (attribute & 3) << 2;

		if (cacheTile == tileAddress && cacheAttribute == attribute) {
			qMemCopy(dst, dst-8, 8*sizeof(QRgb));
			*bgWr = *(bgWr - 1);
		} else {
			cacheTile = tileAddress;
			cacheAttribute = attribute;
			u8 plane1 = nesMapper->ppuRead(tileAddress + 0);
			u8 plane2 = nesMapper->ppuRead(tileAddress + 8);
			*bgWr = plane1 | plane2;

			QRgb *pens = currPens + attribute;
			register int c1 = ((plane1>>1)&0x55) | (plane2&0xAA);
			register int c2 = (plane1&0x55) | ((plane2<<1)&0xAA);
			dst[0] = pens[(c1>>6)];
			dst[4] = pens[(c1>>2)&3];
			dst[1] = pens[(c2>>6)];
			dst[5] = pens[(c2>>2)&3];
			dst[2] = pens[(c1>>4)&3];
			dst[6] = pens[c1&3];
			dst[3] = pens[(c2>>4)&3];
			dst[7] = pens[c2&3];
		}
		dst += 8;
		bgWr++;

		/* character latch (for MMC2/MMC4) */
		if (characterLatchEnabled)
			nesMapper->characterLatch(tileAddress);

		if (++nameTableX == 32) {
			nameTableX = 0;
			nameTableAddress ^= 0x41F;
			attributeAddress = AttributeTableOffset | ((nameTableAddress&0x0380)>>4);
			nameTable = nesMapper->ppuBank1KData(nameTableAddress >> 10);
		} else {
			nameTableAddress++;
		}
	}
}

void NesPpu::drawBackgroundNoTileExtLatch() {
	QRgb *dst = scanlineData + (8 - loopyShift);

	u16 nameTableAddress = NameTableOffset | (nesVramAddress & 0x0FFF);
	u8 nameTableX = nameTableAddress & 0x001F;

	u32 cacheTile = 0xFFFF0000;
	u8 cacheAttribute = 0xFF;

	u8 *bgWr = bgWritten;
	QRgb *currPens = currentPens();
	for (int i = 0; i < 33; i++) {
		nesMapper->extensionLatchX(i);
		u8 plane1, plane2;
		u8 attribute;
		nesMapper->extensionLatch(nameTableAddress, &plane1, &plane2, &attribute);
		attribute &= 0x0C;
		u32 tile = (plane1 << 8) | plane2;
		if (cacheTile == tile && cacheAttribute == attribute) {
			qMemCopy(dst, dst-8, 8*sizeof(QRgb));
			*bgWr = *(bgWr - 1);
		} else {
			cacheTile = tile;
			cacheAttribute = attribute;
			*bgWr = plane1 | plane2;

			QRgb *pens = currPens + attribute;
			register int c1 = ((plane1>>1)&0x55) | (plane2&0xAA);
			register int c2 = (plane1&0x55) | ((plane2<<1)&0xAA);
			dst[0] = pens[(c1>>6)];
			dst[4] = pens[(c1>>2)&3];
			dst[1] = pens[(c2>>6)];
			dst[5] = pens[(c2>>2)&3];
			dst[2] = pens[(c1>>4)&3];
			dst[6] = pens[c1&3];
			dst[3] = pens[(c2>>4)&3];
			dst[7] = pens[c2&3];
		}
		dst += 8;
		bgWr++;

		if (++nameTableX == 32) {
			nameTableX = 0;
			nameTableAddress ^= 0x41F;
		} else {
			nameTableAddress++;
		}
	}
}

void NesPpu::drawBackgroundTileExtLatch() {
	QRgb *dst = scanlineData + (8 - loopyShift);

	u16 nameTableAddress = NameTableOffset | (nesVramAddress & 0x0FFF);
	u8 nameTableX = nameTableAddress & 0x001F;

	u32 cacheTile = 0xFFFF0000;
	u8 cacheAttribute = 0xFF;

	u8 *bgWr = bgWritten;
	QRgb *currPens = currentPens();
	for (int i = 0; i < 33; i++) {
		if (i)
			nesMachine.clockCpu(FetchCycles*4);
		nesMapper->extensionLatchX(i);
		u8 plane1, plane2;
		u8 attribute;
		nesMapper->extensionLatch(nameTableAddress, &plane1, &plane2, &attribute);
		attribute &= 0x0C;
		u32 tile = (plane1 << 8) | plane2;
		if (cacheTile == tile && cacheAttribute == attribute) {
			qMemCopy(dst, dst-8, 8*sizeof(QRgb));
			*bgWr = *(bgWr - 1);
		} else {
			cacheTile = tile;
			cacheAttribute = attribute;
			*bgWr = plane1 | plane2;

			QRgb *pens = currPens + attribute;
			register int c1 = ((plane1>>1)&0x55) | (plane2&0xAA);
			register int c2 = (plane1&0x55) | ((plane2<<1)&0xAA);
			dst[0] = pens[(c1>>6)];
			dst[4] = pens[(c1>>2)&3];
			dst[1] = pens[(c2>>6)];
			dst[5] = pens[(c2>>2)&3];
			dst[2] = pens[(c1>>4)&3];
			dst[6] = pens[c1&3];
			dst[3] = pens[(c2>>4)&3];
			dst[7] = pens[c2&3];
		}
		dst += 8;
		bgWr++;

		if (++nameTableX == 32) {
			nameTableX = 0;
			nameTableAddress ^= 0x41F;
		} else {
			nameTableAddress++;
		}
	}
}

static inline bool sprite0HitOccurred()
{ return nesPpuRegs[NesPpu::Status] & NesPpu::Sprite0HitSRBit; }

void NesPpu::drawSprites() {
	setSpriteMax(false);
	if (nesPpuScanline >= VisibleScreenHeight || !isSpriteVisible())
		return;

	u8 spWritten[33];
	qMemSet(spWritten, 0, sizeof(spWritten));
	if (!(nesPpuRegs[Control1] & SpriteClipDisableCR1Bit))
		spWritten[0] = 0xFF;

	QRgb *currPens = currentPens();
	int count = 0;
	const NesPpuSprite *sprite = (const NesPpuSprite *)spriteMem;
	for (int spriteIndex = 0; spriteIndex < NumSprites; spriteIndex++, sprite++) {
		/* compute the character's line to draw */
		uint spriteLine = nesPpuScanline - sprite->y();
		/* if the sprite isn't visible, skip it */
		if (spriteLine != (spriteLine & (spriteSize-1)))
			continue;
		/* compute character pattern address */
		int tile = sprite->tileIndex();
		if (spriteSize == 16 && (tile & 1)) {
			/* if it's 8x16 and odd-numbered, draw the other half instead */
			tile &= ~1;
			tile |= 0x100;
		}
		if (sprite->flipVertically())
			spriteLine = (spriteSize-1) - spriteLine;
		/* if it's 8x16 and line >= 8 move to next tile */
		if (spriteLine & 8) {
			tile++;
			spriteLine &= ~8;
		}
		int index1 = tile * 16;
		if (spriteSize == 8)
			index1 |= spritePageOffset;
		u16 spriteAddress = index1 | spriteLine;
		/* read character pattern */
		u8 plane1 = nesMapper->ppuRead(spriteAddress + 0);
		u8 plane2 = nesMapper->ppuRead(spriteAddress + 8);
		/* character latch (for MMC2/MMC4) */
		if (characterLatchEnabled)
			nesMapper->characterLatch(spriteAddress);
		if (sprite->flipHorizontally()) {
			plane1 = bit2Rev[plane1];
			plane2 = bit2Rev[plane2];
		}
		u8 pixelData = plane1 | plane2;
		/* set the "sprite 0 hit" flag if appropriate */
		if (!spriteIndex && !sprite0HitOccurred()) {
			int backgroundPos = ((sprite->x()&0xF8)+((loopyShift+(sprite->x()&0x07))&8))>>3;
			int backgroundShift = 8-((loopyShift+sprite->x())&7);
			u8 backgroundMask = ((bgWritten[backgroundPos+0]<<8)|bgWritten[backgroundPos+1]) >> backgroundShift;
			if (pixelData & backgroundMask)
				nesPpuRegs[Status] |= Sprite0HitSRBit;
		}
		/* sprite mask */
		int spritePos = sprite->x()/8;
		int spriteShift = 8-(sprite->x()&7);
		u8 spriteMask = ((spWritten[spritePos+0]<<8)|spWritten[spritePos+1]) >> spriteShift;
		u16 toWrite = pixelData << spriteShift;
		spWritten[spritePos+0] |= toWrite >> 8;
		spWritten[spritePos+1] |= toWrite & 0xFF;
		pixelData &= ~spriteMask;

		if (sprite->isBehindBackground()) {
			/* BG > SP priority */
			int backgroundPos = ((sprite->x()&0xF8)+((loopyShift+(sprite->x()&0x07))&8))>>3;
			int backgroundShift = 8-((loopyShift+sprite->x())&7);
			u8 backgroundMask = ((bgWritten[backgroundPos+0]<<8)|bgWritten[backgroundPos+1]) >> backgroundShift;
			pixelData &= ~backgroundMask;
		}
		/* draw */
		QRgb *dst = scanlineData + sprite->x() + 8;
		QRgb *pens = currPens + (sprite->paletteHighBits() | 0x10);
		register int c1 = ((plane1>>1)&0x55) | (plane2&0xAA);
		register int c2 = (plane1&0x55) | ((plane2<<1)&0xAA);
		if (pixelData&0x80) dst[0] = pens[(c1>>6)];
		if (pixelData&0x08) dst[4] = pens[(c1>>2)&3];
		if (pixelData&0x40) dst[1] = pens[(c2>>6)];
		if (pixelData&0x04) dst[5] = pens[(c2>>2)&3];
		if (pixelData&0x20) dst[2] = pens[(c1>>4)&3];
		if (pixelData&0x02) dst[6] = pens[c1&3];
		if (pixelData&0x10) dst[3] = pens[(c2>>4)&3];
		if (pixelData&0x01) dst[7] = pens[c2&3];

		if (++count == 8) {
			setSpriteMax(true);
			break;
		}
	}
}

void NesPpu::fillScanline(int color, int count) {
	QRgb pen = currentPens()[color];
	for (int i = 0; i < count; i++)
		scanlineData[i] = pen;
}

void NesPpu::processDummyScanline() {
	if (nesPpuScanline >= VisibleScreenHeight || !isSpriteVisible())
		return;
	setSpriteMax(false);
	int count = 0;
	const NesPpuSprite *sprite = (const NesPpuSprite *)spriteMem;
	for (int spriteIndex = 0; spriteIndex < NumSprites; spriteIndex++, sprite++) {
		/* compute the character's line to draw */
		uint spriteLine = nesPpuScanline - sprite->y();
		/* if the sprite isn't visible, skip it */
		if (spriteLine != (spriteLine & (spriteSize-1)))
			continue;
		if (++count == 8) {
			setSpriteMax(true);
			break;
		}
	}
}

bool NesPpu::checkSprite0HitHere() const {
	if (sprite0HitOccurred())
		return false;
	if (!isBackgroundVisible() || !isSpriteVisible())
		return false;
	const NesPpuSprite *sprite = (const NesPpuSprite *)spriteMem;
	/* compute the character's line to draw */
	uint spriteLine = nesPpuScanline - sprite->y();
	/* if the sprite isn't visible, skip it */
	if (spriteLine != (spriteLine & (spriteSize-1)))
		return false;
	return true;
}

static void fillPens() {
	/* This routine builds a palette using a transformation from */
	/* the YUV (Y, B-Y, R-Y) to the RGB color space */

	/* The NES has a 64 color palette                        */
	/* 16 colors, with 4 luminance levels for each color     */
	/* The 16 colors circle around the YUV color space,      */

	int entry = 0;
	qreal tint = 0.22f;	/* adjust to taste */
	qreal hue = 287.0f;

	qreal Kr = 0.2989f;
	qreal Kb = 0.1145f;
	qreal Ku = 2.029f;
	qreal Kv = 1.140f;

	static const qreal brightness[3][4] = {
		{ 0.50f, 0.75f,  1.0f,  1.0f },
		{ 0.29f, 0.45f, 0.73f,  0.9f },
		{ 0.0f,  0.24f, 0.47f, 0.77f }
	};
	/* Loop through the emphasis modes (8 total) */
	for (int colorEmphasis = 0; colorEmphasis < 8; colorEmphasis++) {
		/* loop through the 4 intensities */
		for (int colorIntensity = 0; colorIntensity < 4; colorIntensity++) {
			/* loop through the 16 colors */
			for (int colorNum = 0; colorNum < 16; colorNum++) {
				qreal sat;
				qreal y, u, v;
				qreal rad;

				switch (colorNum) {
				case 0:
					sat = 0.0f; rad = 0.0f;
					y = brightness[0][colorIntensity];
					break;
				case 13:
					sat = 0.0f; rad = 0.0f;
					y = brightness[2][colorIntensity];
					break;
				case 14:
				case 15:
					sat = 0.0f; rad = 0.0f; y = 0.0f;
					break;
				default:
					sat = tint;
					rad = M_PI * (qreal(qreal(colorNum) * 30.0f + hue) / 180.0f);
					y = brightness[1][colorIntensity];
					break;
				}
				u = sat * qCos(rad);
				v = sat * qSin(rad);
				/* Transform to RGB */
				qreal R = (y + Kv * v) * 255.0f;
				qreal G = (y - (Kb * Ku * u + Kr * Kv * v) / (1 - Kb - Kr)) * 255.0f;
				qreal B = (y + Ku * u) * 255.0f;
				/* Clipping, in case of saturation */
				R = qMax(qreal(0.0f), qMin(R, qreal(255.0f)));
				G = qMax(qreal(0.0f), qMin(G, qreal(255.0f)));
				B = qMax(qreal(0.0f), qMin(B, qreal(255.0f)));
				/* emphasis */
				R = ((colorEmphasis & 1) ? 255.0f : R);
				G = ((colorEmphasis & 2) ? 255.0f : G);
				B = ((colorEmphasis & 4) ? 255.0f : B);
				/* Round, and set the value */
				palettePens[entry++] = qRgb(qFloor(R + 0.5f), qFloor(G + 0.5f), qFloor(B + 0.5f));
			}
		}
	}
}

#define STATE_SERIALIZE_BUILDER(sl) \
STATE_SERIALIZE_BEGIN_##sl(NesPpu, 1) \
	u8 type_ = ppuType; \
	u8 renderMethod_ = ppuRenderMethod; \
	STATE_SERIALIZE_VAR_##sl(type_) \
	STATE_SERIALIZE_VAR_##sl(nesPpuScanlinesPerFrame) \
	STATE_SERIALIZE_VAR_##sl(renderMethod_) \
	STATE_SERIALIZE_VAR_##sl(nesVramAddress) \
	STATE_SERIALIZE_VAR_##sl(refreshLatch) \
	STATE_SERIALIZE_VAR_##sl(scrollTileXOffset) \
	STATE_SERIALIZE_VAR_##sl(nesPpuScrollTileYOffset) \
	STATE_SERIALIZE_VAR_##sl(nesPpuTilePageOffset) \
	STATE_SERIALIZE_VAR_##sl(spritePageOffset) \
	STATE_SERIALIZE_VAR_##sl(loopyShift) \
	STATE_SERIALIZE_VAR_##sl(vBlankOut) \
	STATE_SERIALIZE_ARRAY_##sl(spriteMem, sizeof(spriteMem)) \
	ppuType = static_cast<ChipType>(type_); \
	ppuRenderMethod = static_cast<RenderMethod>(renderMethod_); \
	\
	STATE_SERIALIZE_ARRAY_##sl(nesPpuRegs, 4) \
	STATE_SERIALIZE_VAR_##sl(regToggle) \
	STATE_SERIALIZE_VAR_##sl(dataLatch) \
	STATE_SERIALIZE_VAR_##sl(incrementValue) \
	STATE_SERIALIZE_VAR_##sl(bufferedData) \
	STATE_SERIALIZE_VAR_##sl(securityValue) \
	\
	STATE_SERIALIZE_ARRAY_##sl(paletteMem, sizeof(paletteMem)) \
	STATE_SERIALIZE_VAR_##sl(paletteMask) \
	STATE_SERIALIZE_VAR_##sl(paletteEmphasis) \
	palettePenLutNeedsRebuild = true; \
STATE_SERIALIZE_END_##sl(NesPpu)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
