#include "nesppu.h"
#include "nesppupalette.h"
#include "nesppuregisters.h"
#include "nesmapper.h"
#include "nesppusprite.h"
#include "nesmachine.h"
#include <QDataStream>

NesPpu::NesPpu(NesMachine *machine) :
	QObject(machine),
	m_mapper(0),
	m_frame(8+VisibleScreenWidth+8, VisibleScreenHeight, QImage::Format_RGB32) {

	m_registers = new NesPpuRegisters(this);
	m_palette = new NesPpuPalette(this);

	setChipType(PPU2C02);
	setRenderMethod(PreRender);

	m_scanline = 0;
	m_scanlineData = 0;
	m_scanline0Data = reinterpret_cast<QRgb *>(m_frame.scanLine(0));

	m_characterLatchEnabled = false;
	m_externalLatchEnabled = false;

	m_vBlankOut = false;

	m_vramAddress = 0;
	m_refreshLatch = 0;
	m_scrollTileXOffset = 0;
	m_scrollTileYOffset = 0;
	m_tilePageOffset = 0;
	m_spritePageOffset = 0;
	m_loopyShift = 0;

	for (int i = 0; i < 256; i++) {
		quint8 m = 0x80;
		quint8 c = 0;
		for (int j = 0; j < 8; j++) {
			if (i & (1<<j))
				c |= m;
			m >>= 1;
		}
		m_bit2Rev[i] = c;
	}

	qMemSet(m_spriteMemory, 0, sizeof(m_spriteMemory));

	m_spriteClippingEnable = true;
}

NesPpu::~NesPpu() {
}

NesMachine *NesPpu::machine() const
{ return static_cast<NesMachine *>(parent()); }

void NesPpu::setMapper(NesMapper *mapper)
{ m_mapper = mapper; }

void NesPpu::setChipType(ChipType newType) {
	m_type = newType;
	if (m_type == PPU2C07)
		m_scanlinesPerFrame = ScanlinesPerFramePAL;
	else
		m_scanlinesPerFrame = ScanlinesPerFrameNTSC;
	m_registers->updateType();
}

void NesPpu::setRenderMethod(NesPpu::RenderMethod method) {
	if (m_renderMethod != method) {
		m_renderMethod = method;
		emit renderMethodChanged();
	}
}

void NesPpu::setScanline(int line) {
	m_scanline = line;
	if (m_scanline < VisibleScreenHeight)
		m_scanlineData = reinterpret_cast<QRgb *>(reinterpret_cast<uchar *>(m_scanline0Data) + line * m_frame.bytesPerLine());
}

void NesPpu::updateVBlankOut() {
	bool newVBlank = (m_registers->isVBlankEnabled() && m_registers->isVBlank());
	if (newVBlank != m_vBlankOut) {
		m_vBlankOut = newVBlank;
		emit vblank_o(m_vBlankOut);
	}
}

void NesPpu::dma(quint8 page) {
	quint16 address = page << 8;
	for (int i = 0; i < 256; i++)
		m_registers->write(NesPpuRegisters::SpriteRAMIO, m_mapper->read(address + i));
}

void NesPpu::setCharacterLatchEnabled(bool on)
{ m_characterLatchEnabled = on; }
void NesPpu::setExternalLatchEnabled(bool on)
{ m_externalLatchEnabled = on; }

void NesPpu::processFrameStart() {
	if (m_registers->isDisplayOn()) {
		m_vramAddress = m_refreshLatch;
		m_loopyShift = m_scrollTileXOffset;
		m_scrollTileYOffset = (m_vramAddress & 0x7000) >> 12;
	}
}

void NesPpu::processFrameEnd() {
	m_frame.detach();
}

void NesPpu::processScanlineStart() {
	if (m_registers->isDisplayOn()) {
		m_vramAddress = (m_vramAddress & 0xFBE0) | (m_refreshLatch & 0x041F);
		m_loopyShift = m_scrollTileXOffset;
		m_scrollTileYOffset = (m_vramAddress & 0x7000) >> 12;
		m_mapper->addressBusLatch(NameTableOffset | (m_vramAddress & 0x0FFF));
	}
}

void NesPpu::processScanlineNext() {
	if (m_registers->isDisplayOn()) {
		if ((m_vramAddress & 0x7000) == 0x7000) {
			m_vramAddress &= 0x8FFF;
			if ((m_vramAddress & 0x03E0) == 0x03A0) {
				m_vramAddress ^= 0x0800;
				m_vramAddress &= 0xFC1F;
			} else {
				if ((m_vramAddress & 0x03E0) == 0x03E0) {
					m_vramAddress &= 0xFC1F;
				} else {
					m_vramAddress += 0x0020;
				}
			}
		} else {
			m_vramAddress += 0x1000;
		}
		m_scrollTileYOffset = (m_vramAddress & 0x7000) >> 12;
	}
}

void NesPpu::processScanline() {
	drawBackground();
	drawSprites();
}

void NesPpu::drawBackground() {
	qMemSet(m_bgWritten, 0, sizeof(m_bgWritten));
	if (!m_registers->isBackgroundVisible()) {
		fillScanline(0, 8+VisibleScreenWidth);
		if (m_renderMethod == TileRender)
			machine()->clockCpu(FetchCycles*4*32);
		return;
	}
	if (m_renderMethod != TileRender) {
		if (!m_externalLatchEnabled)
			drawBackgroundNoTileNoExtLatch();
		else
			drawBackgroundNoTileExtLatch();
	} else {
		if (!m_externalLatchEnabled)
			drawBackgroundTileNoExtLatch();
		else
			drawBackgroundTileExtLatch();
	}
	/* if the left 8 pixels for the background are off, blank them */
	if (m_registers->isBackgroundClippingEnabled()) {
		fillScanline(0, 16);
		// TODO m_bgWritten[0] = 0; virtuanes-
	}
}

void NesPpu::drawBackgroundNoTileNoExtLatch() {
	QRgb *dst = m_scanlineData + (8 - m_loopyShift);

	int nameTableAddress = NameTableOffset | (m_vramAddress & 0x0FFF);
	int attributeAddress = AttributeTableOffset | ((nameTableAddress&0x0380)>>4);
	int nameTableX = nameTableAddress & 0x001F;
	int attributeShift = (nameTableAddress & 0x0040) >> 4;
	quint8 *nameTable = m_mapper->ppuBank1KData(nameTableAddress >> 10);

	int cacheTile = 0xFFFF0000;
	quint8 cacheAttribute = 0xFF;

	attributeAddress &= 0x3FF;

	quint8 *bgWritten = m_bgWritten;
	QRgb *currentPens = m_palette->currentPens();
	for (int i = 0; i < 33; i++) {
		int tileAddress = nameTable[nameTableAddress & 0x03FF] * 0x10;
		tileAddress += m_tilePageOffset + m_scrollTileYOffset;
		quint8 attribute = nameTable[attributeAddress + (nameTableX >> 2)];
		attribute >>= (nameTableX & 2) | attributeShift;
		attribute = (attribute & 3) << 2;

		if (cacheTile == tileAddress && cacheAttribute == attribute) {
			// FIXME
//			qMemCopy(dst, dst-8, 8*sizeof(QRgb));
			memcpy(dst, dst-8, 8*sizeof(QRgb));
			*bgWritten = *(bgWritten - 1);
		} else {
			cacheTile = tileAddress;
			cacheAttribute = attribute;
			quint8 plane1 = m_mapper->ppuRead(tileAddress + 0);
			quint8 plane2 = m_mapper->ppuRead(tileAddress + 8);
			*bgWritten = plane1 | plane2;

			QRgb *pens = currentPens + attribute;
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
		bgWritten++;

		/* character latch (for MMC2/MMC4) */
		if (m_characterLatchEnabled)
			m_mapper->characterLatch(tileAddress);

		if (++nameTableX == 32) {
			nameTableX = 0;
			nameTableAddress ^= 0x41F;
			attributeAddress = AttributeTableOffset | ((nameTableAddress&0x0380)>>4);
			nameTable = m_mapper->ppuBank1KData(nameTableAddress >> 10);
		} else {
			nameTableAddress++;
		}
	}
}

void NesPpu::drawBackgroundTileNoExtLatch() {
	QRgb *dst = m_scanlineData + (8 - m_loopyShift);

	int nameTableAddress = NameTableOffset | (m_vramAddress & 0x0FFF);
	int attributeAddress = AttributeTableOffset | ((nameTableAddress&0x0380)>>4);
	int nameTableX = nameTableAddress & 0x001F;
	int attributeShift = (nameTableAddress & 0x0040) >> 4;
	quint8 *nameTable = m_mapper->ppuBank1KData(nameTableAddress >> 10);

	int cacheTile = 0xFFFF0000;
	quint8 cacheAttribute = 0xFF;

	attributeAddress &= 0x3FF;

	quint8 *bgWritten = m_bgWritten;
	QRgb *currentPens = m_palette->currentPens();
	for (int i = 0; i < 33; i++) {
		int tileAddress = nameTable[nameTableAddress & 0x03FF] * 0x10;
		tileAddress += m_tilePageOffset + m_scrollTileYOffset;
		if (i)
			machine()->clockCpu(FetchCycles*4);
		quint8 attribute = nameTable[attributeAddress + (nameTableX >> 2)];
		attribute >>= (nameTableX & 2) | attributeShift;
		attribute = (attribute & 3) << 2;

		if (cacheTile == tileAddress && cacheAttribute == attribute) {
			qMemCopy(dst, dst-8, 8*sizeof(QRgb));
			*bgWritten = *(bgWritten - 1);
		} else {
			cacheTile = tileAddress;
			cacheAttribute = attribute;
			quint8 plane1 = m_mapper->ppuRead(tileAddress + 0);
			quint8 plane2 = m_mapper->ppuRead(tileAddress + 8);
			*bgWritten = plane1 | plane2;

			QRgb *pens = currentPens + attribute;
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
		bgWritten++;

		/* character latch (for MMC2/MMC4) */
		if (m_characterLatchEnabled)
			m_mapper->characterLatch(tileAddress);

		if (++nameTableX == 32) {
			nameTableX = 0;
			nameTableAddress ^= 0x41F;
			attributeAddress = AttributeTableOffset | ((nameTableAddress&0x0380)>>4);
			nameTable = m_mapper->ppuBank1KData(nameTableAddress >> 10);
		} else {
			nameTableAddress++;
		}
	}
}

void NesPpu::drawBackgroundNoTileExtLatch() {
	QRgb *dst = m_scanlineData + (8 - m_loopyShift);

	int nameTableAddress = NameTableOffset | (m_vramAddress & 0x0FFF);
	int nameTableX = nameTableAddress & 0x001F;

	int cacheTile = 0xFFFF0000;
	quint8 cacheAttribute = 0xFF;

	quint8 *bgWritten = m_bgWritten;
	QRgb *currentPens = m_palette->currentPens();
	for (int i = 0; i < 33; i++) {
		m_mapper->extensionLatchX(i);
		quint8 plane1, plane2;
		quint8 attribute;
		m_mapper->extensionLatch(nameTableAddress, &plane1, &plane2, &attribute);
		attribute &= 0x0C;
		int tile = (plane1 << 8) | plane2;
		if (cacheTile == tile && cacheAttribute == attribute) {
			qMemCopy(dst, dst-8, 8*sizeof(QRgb));
			*bgWritten = *(bgWritten - 1);
		} else {
			cacheTile = tile;
			cacheAttribute = attribute;
			*bgWritten = plane1 | plane2;

			QRgb *pens = currentPens + attribute;
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
		bgWritten++;

		if (++nameTableX == 32) {
			nameTableX = 0;
			nameTableAddress ^= 0x41F;
		} else {
			nameTableAddress++;
		}
	}
}

void NesPpu::drawBackgroundTileExtLatch() {
	QRgb *dst = m_scanlineData + (8 - m_loopyShift);

	int nameTableAddress = NameTableOffset | (m_vramAddress & 0x0FFF);
	int nameTableX = nameTableAddress & 0x001F;

	int cacheTile = 0xFFFF0000;
	quint8 cacheAttribute = 0xFF;

	quint8 *bgWritten = m_bgWritten;
	QRgb *currentPens = m_palette->currentPens();
	for (int i = 0; i < 33; i++) {
		if (i)
			machine()->clockCpu(FetchCycles*4);
		m_mapper->extensionLatchX(i);
		quint8 plane1, plane2;
		quint8 attribute;
		m_mapper->extensionLatch(nameTableAddress, &plane1, &plane2, &attribute);
		attribute &= 0x0C;
		int tile = (plane1 << 8) | plane2;
		if (cacheTile == tile && cacheAttribute == attribute) {
			qMemCopy(dst, dst-8, 8*sizeof(QRgb));
			*bgWritten = *(bgWritten - 1);
		} else {
			cacheTile = tile;
			cacheAttribute = attribute;
			*bgWritten = plane1 | plane2;

			QRgb *pens = currentPens + attribute;
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
		bgWritten++;

		if (++nameTableX == 32) {
			nameTableX = 0;
			nameTableAddress ^= 0x41F;
		} else {
			nameTableAddress++;
		}
	}
}

void NesPpu::drawSprites() {
	m_registers->setSpriteMax(false);
	if (m_scanline >= VisibleScreenHeight || !m_registers->isSpriteVisible())
		return;

	quint8 spWritten[33];
	qMemSet(spWritten, 0, sizeof(spWritten));
	if (m_registers->isSpriteClippingEnabled() && m_spriteClippingEnable)
		spWritten[0] = 0xFF;

	QRgb *currentPens = m_palette->currentPens();
	int count = 0;
	int spriteSize = m_registers->spriteSize();
	NesPpuSprite *sprite = reinterpret_cast<NesPpuSprite *>(m_spriteMemory);
	for (int spriteIndex = 0; spriteIndex < NumSprites; spriteIndex++, sprite++) {
		/* compute the character's line to draw */
		int spriteLine = m_scanline - sprite->y();
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
			index1 |= m_spritePageOffset;
		quint16 spriteAddress = index1 | spriteLine;
		/* read character pattern */
		quint8 plane1 = m_mapper->ppuRead(spriteAddress + 0);
		quint8 plane2 = m_mapper->ppuRead(spriteAddress + 8);
		/* character latch (for MMC2/MMC4) */
		if (m_characterLatchEnabled)
			m_mapper->characterLatch(spriteAddress);
		if (sprite->flipHorizontally()) {
			plane1 = m_bit2Rev[plane1];
			plane2 = m_bit2Rev[plane2];
		}
		quint8 pixelData = plane1 | plane2;
		/* set the "sprite 0 hit" flag if appropriate */
		if (!spriteIndex && !m_registers->sprite0HitOccurred()) {
			int backgroundPos = ((sprite->x()&0xF8)+((m_loopyShift+(sprite->x()&0x07))&8))>>3;
			int backgroundShift = 8-((m_loopyShift+sprite->x())&7);
			quint8 backgroundMask = ((m_bgWritten[backgroundPos+0]<<8)|m_bgWritten[backgroundPos+1]) >> backgroundShift;
			if (pixelData & backgroundMask)
				m_registers->setSprite0Hit();
		}
		/* sprite mask */
		int spritePos = sprite->x()/8;
		int spriteShift = 8-(sprite->x()&7);
		quint8 spriteMask = ((spWritten[spritePos+0]<<8)|spWritten[spritePos+1]) >> spriteShift;
		quint16 toWrite = pixelData << spriteShift;
		spWritten[spritePos+0] |= toWrite >> 8;
		spWritten[spritePos+1] |= toWrite & 0xFF;
		pixelData &= ~spriteMask;

		if (sprite->isBehindBackground()) {
			/* BG > SP priority */
			int backgroundPos = ((sprite->x()&0xF8)+((m_loopyShift+(sprite->x()&0x07))&8))>>3;
			int backgroundShift = 8-((m_loopyShift+sprite->x())&7);
			quint8 backgroundMask = ((m_bgWritten[backgroundPos+0]<<8)|m_bgWritten[backgroundPos+1]) >> backgroundShift;
			pixelData &= ~backgroundMask;
		}
		/* draw */
		QRgb *dst = m_scanlineData + sprite->x() + 8;
		QRgb *pens = currentPens + (sprite->paletteHighBits() | 0x10);
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
			m_registers->setSpriteMax(true);
			break;
		}
	}
}

void NesPpu::fillScanline(int color, int count) {
	QRgb pen = m_palette->currentPens()[color];
	for (int i = 0; i < count; i++)
		m_scanlineData[i] = pen;
}

void NesPpu::processDummyScanline() {
	if (m_scanline >= VisibleScreenHeight || !m_registers->isSpriteVisible())
		return;
	m_registers->setSpriteMax(false);
	int count = 0;
	int spriteSize = m_registers->spriteSize();
	NesPpuSprite *sprite = reinterpret_cast<NesPpuSprite *>(m_spriteMemory);
	for (int spriteIndex = 0; spriteIndex < NumSprites; spriteIndex++, sprite++) {
		/* compute the character's line to draw */
		int spriteLine = m_scanline - sprite->y();
		/* if the sprite isn't visible, skip it */
		if (spriteLine != (spriteLine & (spriteSize-1)))
			continue;
		if (++count == 8) {
			m_registers->setSpriteMax(true);
			break;
		}
	}
}

bool NesPpu::checkSprite0HitHere() const {
	if (m_registers->sprite0HitOccurred())
		return false;
	if (!m_registers->isBackgroundVisible() || !m_registers->isSpriteVisible())
		return false;
	int spriteSize = m_registers->spriteSize();
	const NesPpuSprite *sprite = reinterpret_cast<const NesPpuSprite *>(m_spriteMemory);
	/* compute the character's line to draw */
	int spriteLine = m_scanline - sprite->y();
	/* if the sprite isn't visible, skip it */
	if (spriteLine != (spriteLine & (spriteSize-1)))
		return false;
	return true;
}

void NesPpu::setSpriteClippingEnabled(bool on) {
	if (m_spriteClippingEnable != on) {
		m_spriteClippingEnable = on;
		emit spriteClippingEnableChanged();
	}
}

#define STATE_SERIALIZE_BUILDER(sl) \
	STATE_SERIALIZE_BEGIN_##sl(NesPpu) \
	quint8 type_ = m_type; \
	quint8 renderMethod_ = m_renderMethod; \
	STATE_SERIALIZE_SUBCALL_PTR_##sl(m_registers) \
	STATE_SERIALIZE_SUBCALL_PTR_##sl(m_palette) \
	STATE_SERIALIZE_SUBCALL_PTR_##sl(m_mapper) \
	STATE_SERIALIZE_VAR_##sl(type_) \
	STATE_SERIALIZE_VAR_##sl(m_scanlinesPerFrame) \
	STATE_SERIALIZE_VAR_##sl(renderMethod_) \
	STATE_SERIALIZE_VAR_##sl(m_characterLatchEnabled) \
	STATE_SERIALIZE_VAR_##sl(m_externalLatchEnabled) \
	STATE_SERIALIZE_VAR_##sl(m_vramAddress) \
	STATE_SERIALIZE_VAR_##sl(m_refreshLatch) \
	STATE_SERIALIZE_VAR_##sl(m_scrollTileXOffset) \
	STATE_SERIALIZE_VAR_##sl(m_scrollTileYOffset) \
	STATE_SERIALIZE_VAR_##sl(m_tilePageOffset) \
	STATE_SERIALIZE_VAR_##sl(m_spritePageOffset) \
	STATE_SERIALIZE_VAR_##sl(m_loopyShift) \
	STATE_SERIALIZE_VAR_##sl(m_vBlankOut) \
	STATE_SERIALIZE_ARRAY_##sl(m_spriteMemory, sizeof(m_spriteMemory)) \
	m_type = static_cast<ChipType>(type_); \
	m_renderMethod = static_cast<RenderMethod>(renderMethod_); \
	STATE_SERIALIZE_END(NesPpu)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
