/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "ppu.h"
#include "mapper.h"
#include "nes.h"
#include "cpu.h"
#include <configuration.h>
#include <qmath.h>
#include <QDataStream>

static const u8 paletteDefaultMem[] =
{
	0,	1,	2,	3,
	0,	5,	6,	7,
	0,	9,	10,	11,
	0,	13,	14,	15,
	0,	17,	18,	19,
	0,	21,	22,	23,
	0,	25,	26,	27,
	0,	29,	30,	31
};

static const char *spriteLimitConfName = "nes.ppu.spriteLimit";

QImage nesPpuFrame;
NesPpu nesPpu;
NesPpuScroll nesPpuScroll;

u8 nesPpuRegs[8]; // registers at 0x2000-0x2007
static u8 dataLatch;
static u8 bufferedData;
static u8 securityValue;

int nesPpuScanline;
static QRgb *scanlineData;

static bool characterLatchEnabled = false;
static bool externalLatchEnabled = false;

bool vBlankOut;

u16 nesPpuTilePageOffset;
static u16 spritePageOffset;
static u16 scrollAddressIncrement;
static uint spriteSize;

static u8 bgWritten[33+3];// 3 bytes pad
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
static void drawSprites();
static void drawBackground();
static void drawBackgroundNoTileNoExtLatch();
static void drawBackgroundNoTileExtLatch();
static void drawBackgroundTileNoExtLatch();
static void drawBackgroundTileExtLatch();
static bool ppuSpriteLimit;

static void buildPenLUT()
{
	for (int i = 0; i < 32; i++)
		palettePenLut[i] = palettePens[(paletteMem[i] & paletteMask) + paletteEmphasis];
}

static void updateColorEmphasisAndMask()
{
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

static inline void updateCachedControl0Bits()
{
	/* the char ram bank points either 0x0000 or 0x1000 (page 0 or page 4) */
	u8 data = nesPpuRegs[NesPpu::Control0];
	nesPpuTilePageOffset = (data & NesPpu::BackgroundTableCR0Bit) << 8;
	spritePageOffset = (data & NesPpu::SpriteTableCR0Bit) << 9;
	scrollAddressIncrement = ((data & NesPpu::IncrementCR0Bit) ? 32 : 1);
	spriteSize = ((data & NesPpu::SpriteSizeCR0Bit) ? 16 : 8);
}

static void setChipType(NesPpu::ChipType type)
{
	switch (type) {
	case NesPpu::PPU2C05_01:	securityValue = 0x1B; break;
	case NesPpu::PPU2C05_02:	securityValue = 0x3D; break;
	case NesPpu::PPU2C05_03:	securityValue = 0x1C; break;
	case NesPpu::PPU2C05_04:	securityValue = 0x1B; break;
	default:					securityValue = 0x00; break;
	}
}

void nesPpuInit()
{
	nesPpuFrame = QImage(8+NesPpu::VisibleScreenWidth+8,
						 NesPpu::VisibleScreenHeight,
						 QImage::Format_RGB32);

	memset(nesPpuRegs, 0, sizeof(nesPpuRegs));
	memset(spriteMem, 0, sizeof(spriteMem));
	updateCachedControl0Bits();

	securityValue = 0;
	dataLatch = 0;
	bufferedData = 0;

	nesPpuScanline = 0;
	scanlineData = 0;

	characterLatchEnabled = false;
	externalLatchEnabled = false;

	vBlankOut = false;

	nesPpuScroll.address = 0;
	nesPpuScroll.latch = 0;
	nesPpuScroll.toggle = 0;
	nesPpuScroll.xFine = 0;

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

	if (nesSystemType == NES_PAL)
		setChipType(NesPpu::PPU2C07);
	else
		setChipType(NesPpu::PPU2C02);

	fillPens();
	memcpy(paletteMem, paletteDefaultMem, sizeof(paletteMem));
	updateColorEmphasisAndMask();

	ppuSpriteLimit = emConf.value(spriteLimitConfName, true).toBool();
}

static void writePalette(u16 address, u8 data)
{
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

static u8 readPalette(u16 address)
{
	Q_ASSERT(address < 32);
	return paletteMem[address] & paletteMask;
}

static QRgb *currentPens()
{
	if (palettePenLutNeedsRebuild) {
		buildPenLUT();
		palettePenLutNeedsRebuild = false;
	}
	return palettePenLut;
}

static void updateVBlankOut()
{
	bool vBlankEnabled = (nesPpuRegs[NesPpu::Control0] & NesPpu::VBlankEnableCR0Bit);
	bool vBlankState = (nesPpuRegs[NesPpu::Status] & NesPpu::VBlankSRBit);
	bool newVBlank = (vBlankEnabled && vBlankState);
	if (newVBlank != vBlankOut) {
		vBlankOut = newVBlank;
		nesCpu.nmi_i(vBlankOut);
	}
}

void nesPpuWrite(u16 addr, u8 data)
{
	Q_ASSERT(addr < 8);
	if (securityValue && !(addr & 6))
		addr ^= 1;
	nesPpuRegs[addr] = data;
	switch (static_cast<NesPpu::Register>(addr)) {
	case NesPpu::Control0:
		updateVBlankOut();
		/* update the name table number on our refresh latches */
		nesPpuScroll.latch &= ~(3 << 10);
		nesPpuScroll.latch |= (data & 3) << 10;
		updateCachedControl0Bits();
		break;
	case NesPpu::Control1:
		updateColorEmphasisAndMask();
		break;
	case NesPpu::SpriteRAMAddress:
		break;
	case NesPpu::SpriteRAMIO:
		/* if the PPU is currently rendering the screen,
		 * 0xff is written instead of the desired data. */
		if (nesPpuScanline < NesPpu::VisibleScreenHeight && nesPpuIsSpriteVisible()) {
			data = 0xFF;
		} else {
			if ((nesPpuRegs[NesPpu::SpriteRAMAddress] & 0x03) == 0x02)
				data &= 0xE3;
		}
		spriteMem[nesPpuRegs[NesPpu::SpriteRAMAddress]++] = data;
		break;
	case NesPpu::Scroll:
		if (nesPpuScroll.toggle ^= 1) {
			nesPpuScroll.latch &= ~0x1F;
			nesPpuScroll.latch |= (data >> 3) & 0x1F;
			nesPpuScroll.xFine = data & 7;
		} else {
			nesPpuScroll.latch &= ~((0xF8 << 2) | (7 << 12));
			nesPpuScroll.latch |= (data & 0xF8) << 2;
			nesPpuScroll.latch |= (data & 7) << 12;
		}
		break;
	case NesPpu::VRAMAddress:
		if (nesPpuScroll.toggle ^= 1) {
			nesPpuScroll.latch &= 0x00FF;
			nesPpuScroll.latch |= (data & 0x3F) << 8;
		} else {
			nesPpuScroll.latch &= 0x7F00;
			nesPpuScroll.latch |= data;
			nesPpuScroll.address = nesPpuScroll.latch;
			nesMapper->addressBusLatch(nesPpuScroll.address);
		}
		break;
	case NesPpu::VRAMIO: {
		uint vramAddress = nesPpuScroll.address & 0x3FFF;
		nesPpuScroll.address = (nesPpuScroll.address+scrollAddressIncrement) & 0x7FFF;
		if (vramAddress >= NesPpu::PalettesAddress)
			writePalette(vramAddress & 0x1F, data);
		else if (nesMapper->ppuBank1KType(vramAddress >> 10) != VromBank)
			nesMapper->ppuWrite(vramAddress, data);
		break;
	}
	default:
		break;
	}
	dataLatch = data;
}

u8 nesPpuRead(u16 addr)
{
	Q_ASSERT(addr < 8);
	switch (static_cast<NesPpu::Register>(addr)) {
	case NesPpu::Status:
		/* the top 3 bits of the status register are the only ones that report data. The
		   remainder contain whatever was last in the PPU data latch, except on the RC2C05 (protection) */
		if (securityValue) {
			dataLatch = nesPpuRegs[NesPpu::Status] & (NesPpu::VBlankSRBit|NesPpu::Sprite0HitSRBit);
			dataLatch |= securityValue;
		} else {
			dataLatch = nesPpuRegs[NesPpu::Status] | (dataLatch & 0x1F);
		}
		/* reset hi/lo scroll toggle */
		nesPpuScroll.toggle = 0;
		/* if the vblank bit is set, clear all status bits but the 2 sprite flags */
		if (dataLatch & NesPpu::VBlankSRBit) {
			nesPpuRegs[NesPpu::Status] &= ~NesPpu::VBlankSRBit;
			updateVBlankOut();
		}
		break;
	case NesPpu::SpriteRAMIO:
		dataLatch = spriteMem[nesPpuRegs[NesPpu::SpriteRAMAddress]];
		break;
	case NesPpu::VRAMIO: {
		u16 vramAddress = nesPpuScroll.address & 0x3FFF;
		nesPpuScroll.address = (nesPpuScroll.address+scrollAddressIncrement) & 0x7FFF;
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

void nesPpuSetVBlank(bool on)
{
	if (on)
		nesPpuRegs[NesPpu::Status] |= NesPpu::VBlankSRBit;
	else
		nesPpuRegs[NesPpu::Status] &= ~(NesPpu::VBlankSRBit|NesPpu::Sprite0HitSRBit);
	updateVBlankOut();
}

static inline void setSpriteMax(bool on)
{
	if (on)
		nesPpuRegs[NesPpu::Status] |= NesPpu::SpriteMaxSRBit;
	else
		nesPpuRegs[NesPpu::Status] &= ~NesPpu::SpriteMaxSRBit;
}

void nesPpuNextScanline()
{
	nesPpuScanline++;
	scanlineData += nesPpuFrame.bytesPerLine()/sizeof(QRgb);
}

void nesPpuDma(u8 page)
{
	u16 address = page << 8;
	for (int i = 0; i < 256; i++)
		nesPpuWrite(NesPpu::SpriteRAMIO, nesMapper->read(address + i));
}

void nesPpuSetCharacterLatchEnabled(bool on)
{
	characterLatchEnabled = on;
}

void nesPpuSetExternalLatchEnabled(bool on)
{
	externalLatchEnabled = on;
}

void nesPpuProcessFrameStart()
{
	nesPpuScanline = 0;
	scanlineData = (QRgb *)nesPpuFrame.bits();
	if (nesPpuIsDisplayOn())
		nesPpuScroll.address = nesPpuScroll.latch;
}

static inline u16 fetchNameAddress()
{
	return NesPpu::NameTableOffset | (nesPpuScroll.address & 0x0FFF);
}

void nesPpuProcessScanlineStart()
{
	if (nesPpuIsDisplayOn()) {
		nesPpuScroll.resetX();
		nesMapper->addressBusLatch(fetchNameAddress());
	}
}

void nesPpuProcessScanlineNext()
{
	if (nesPpuIsDisplayOn())
		nesPpuScroll.clockY();
}

void nesPpuProcessScanline()
{
	drawBackground();
	drawSprites();
}

static void fillScanline(int color, int count)
{
	QRgb pen = currentPens()[color];
	memset32(scanlineData, pen, count);
}

static void drawBackground()
{
	memset32(bgWritten, 0, sizeof(bgWritten)/4);
	if (!nesPpuIsBackgroundVisible()) {
		fillScanline(0, 8+NesPpu::VisibleScreenWidth);
		if (nesEmuRenderMethod == NesEmu::TileRender)
			nesEmuClockCpu(NesPpu::FetchCycles*4*32);
		return;
	}
	if (nesEmuRenderMethod != NesEmu::TileRender) {
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
	if (!(nesPpuRegs[NesPpu::Control1] & NesPpu::BackgroundClipDisableCR1Bit))
		fillScanline(0, 16);
}

static void drawBackgroundNoTileNoExtLatch()
{
	QRgb *dst = scanlineData + (8 - nesPpuScroll.xFine);

	u16 nameTableAddress = fetchNameAddress();
	u16 attributeAddress = NesPpu::AttributeTableOffset | ((nameTableAddress&0x0380)>>4);
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
		tileAddress |= nesPpuTilePageOffset | nesPpuScroll.yFine();
		u8 attribute = nameTable[attributeAddress + (nameTableX >> 2)];
		attribute >>= (nameTableX & 2) | attributeShift;
		attribute = (attribute & 3) << 2;

		if (cacheTile == tileAddress && cacheAttribute == attribute) {
			memcpy32(dst, dst-8, 8*sizeof(QRgb)/4);
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
			attributeAddress = NesPpu::AttributeTableOffset | ((nameTableAddress&0x0380)>>4);
			nameTable = nesMapper->ppuBank1KData(nameTableAddress >> 10);
		} else {
			nameTableAddress++;
		}
	}
}

static void drawBackgroundTileNoExtLatch()
{
	QRgb *dst = scanlineData + (8 - nesPpuScroll.xFine);

	u16 nameTableAddress = fetchNameAddress();
	u16 attributeAddress = NesPpu::AttributeTableOffset | ((nameTableAddress&0x0380)>>4);
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
		tileAddress += nesPpuTilePageOffset + nesPpuScroll.yFine();
		if (i)
			nesEmuClockCpu(NesPpu::FetchCycles*4);
		u8 attribute = nameTable[attributeAddress + (nameTableX >> 2)];
		attribute >>= (nameTableX & 2) | attributeShift;
		attribute = (attribute & 3) << 2;

		if (cacheTile == tileAddress && cacheAttribute == attribute) {
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
			attributeAddress = NesPpu::AttributeTableOffset | ((nameTableAddress&0x0380)>>4);
			nameTable = nesMapper->ppuBank1KData(nameTableAddress >> 10);
		} else {
			nameTableAddress++;
		}
	}
}

static void drawBackgroundNoTileExtLatch()
{
	QRgb *dst = scanlineData + (8 - nesPpuScroll.xFine);

	u16 nameTableAddress = fetchNameAddress();
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
			memcpy(dst, dst-8, 8*sizeof(QRgb));
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

static void drawBackgroundTileExtLatch()
{
	QRgb *dst = scanlineData + (8 - nesPpuScroll.xFine);

	u16 nameTableAddress = fetchNameAddress();
	u8 nameTableX = nameTableAddress & 0x001F;

	u32 cacheTile = 0xFFFF0000;
	u8 cacheAttribute = 0xFF;

	u8 *bgWr = bgWritten;
	QRgb *currPens = currentPens();
	for (int i = 0; i < 33; i++) {
		if (i)
			nesEmuClockCpu(NesPpu::FetchCycles*4);
		nesMapper->extensionLatchX(i);
		u8 plane1, plane2;
		u8 attribute;
		nesMapper->extensionLatch(nameTableAddress, &plane1, &plane2, &attribute);
		attribute &= 0x0C;
		u32 tile = (plane1 << 8) | plane2;
		if (cacheTile == tile && cacheAttribute == attribute) {
			memcpy(dst, dst-8, 8*sizeof(QRgb));
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
{
	return nesPpuRegs[NesPpu::Status] & NesPpu::Sprite0HitSRBit;
}

static void drawSprites()
{
	setSpriteMax(false);
	if (nesPpuScanline >= NesPpu::VisibleScreenHeight || !nesPpuIsSpriteVisible())
		return;

	u8 spWritten[33+3]; // 3 bytes pad
	memset32(spWritten, 0, sizeof(spWritten)/4);
	if (!(nesPpuRegs[NesPpu::Control1] & NesPpu::SpriteClipDisableCR1Bit))
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
			int backgroundPos = ((sprite->x()&0xF8)+((nesPpuScroll.xFine+(sprite->x()&0x07))&8))>>3;
			int backgroundShift = 8-((nesPpuScroll.xFine+sprite->x())&7);
			u8 backgroundMask = ((bgWritten[backgroundPos+0]<<8)|bgWritten[backgroundPos+1]) >> backgroundShift;
			if (pixelData & backgroundMask)
				nesPpuRegs[NesPpu::Status] |= NesPpu::Sprite0HitSRBit;
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
			int backgroundPos = ((sprite->x()&0xF8)+((nesPpuScroll.xFine+(sprite->x()&0x07))&8))>>3;
			int backgroundShift = 8-((nesPpuScroll.xFine+sprite->x())&7);
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
			if (ppuSpriteLimit)
				break;
		}
	}
}

void nesPpuProcessDummyScanline()
{
	if (nesPpuScanline >= NesPpu::VisibleScreenHeight || !nesPpuIsSpriteVisible())
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

bool nesPpuCheckSprite0HitHere()
{
	if (sprite0HitOccurred())
		return false;
	if (!nesPpuIsBackgroundVisible() || !nesPpuIsSpriteVisible())
		return false;
	const NesPpuSprite *sprite = (const NesPpuSprite *)spriteMem;
	/* compute the character's line to draw */
	uint spriteLine = nesPpuScanline - sprite->y();
	/* if the sprite isn't visible, skip it */
	if (spriteLine != (spriteLine & (spriteSize-1)))
		return false;
	return true;
}

QRgb nesPpuGetPixel(int x, int y)
{
	Q_ASSERT(x >= 0 && x < NesPpu::VisibleScreenWidth);
	Q_ASSERT(y >= 0 && y < NesPpu::VisibleScreenHeight);
	return nesPpuFrame.pixel(x+8, y);
}

static void fillPens()
{
	/* This routine builds a palette using a transformation from */
	/* the YUV (Y, B-Y, R-Y) to the RGB color space */

	/* The NES has a 64 color palette                        */
	/* 16 colors, with 4 luminance levels for each color     */
	/* The 16 colors circle around the YUV color space,      */

	int entry = 0;
	qreal tint = 0.22f;	/* adjust to taste */ // TODO as an option
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

void nesPpuSl()
{
	emsl.begin("ppu");
	emsl.var("scroll.address", nesPpuScroll.address);
	emsl.var("scroll.latch", nesPpuScroll.latch);
	emsl.var("scroll.toggle", nesPpuScroll.toggle);
	emsl.var("scroll.xFine", nesPpuScroll.xFine);
	emsl.var("scrollAddressIncrement", scrollAddressIncrement);

	emsl.var("vBlankOut", vBlankOut);

	emsl.array("regs", nesPpuRegs, sizeof(nesPpuRegs));
	emsl.var("dataLatch", dataLatch);
	emsl.var("bufferedData", bufferedData);
	emsl.var("securityValue", securityValue);

	emsl.array("spriteMem", spriteMem, sizeof(spriteMem));
	emsl.array("paletteMem", paletteMem, sizeof(paletteMem));
	emsl.end();

	if (!emsl.save) {
		updateCachedControl0Bits();
		updateColorEmphasisAndMask();
		palettePenLutNeedsRebuild = true;
		ppuSpriteLimit = emConf.value(spriteLimitConfName, true).toBool();
	}
}

void NesPpu::setSpriteLimit(bool on)
{
	if (ppuSpriteLimit != on) {
		ppuSpriteLimit = on;
		emConf.setValue(spriteLimitConfName, ppuSpriteLimit);
		emit spriteLimitChanged();
	}
}

bool NesPpu::spriteLimit() const
{
	return ppuSpriteLimit;
}
