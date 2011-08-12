#include "ppu.h"
#include "sprite.h"
#include "palette.h"
#include <QPainter>
#include <QDebug>

void Nes2C0XPpu::drawBackground() {
	if (!m_registers->isBackgroundVisible()) {
		fillScanline(0);
		return;
	}
	QRgb backPen = m_palette->penRgb(0);
	/* determine where in the nametable to start drawing from */
	/* based on the current scanline and scroll regs */
	int scrollTileX = m_refreshData & 0x1F;
	int scrollTileY = (m_refreshData >> 5) & 0x1F;
	int scrollTileYOffset = (m_refreshData >> 12) & 0x07;
	int x = scrollTileX;
	/* get the tile index */
	int tileIndex = ((m_refreshData & 0xC00) | 0x2000) | (scrollTileY << 5);
	/* set up dest */
	int startX = (m_scrollTileXOffset ^ 7) - 7;
	QRgb *dest = reinterpret_cast<QRgb *> (m_frame.scanLine(m_scanline)) + startX;
	/* draw the 32 or 33 tiles that make up a line */
	for (int tilecount = 0; tilecount < 34; tilecount++) {
		int index1 = tileIndex | x;
		// this is attribute table stuff! (actually read 2 in PPUspeak)!
		/* Figure out which byte in the color table to use */
		int pos = ((index1 >> 4) & 0x38) | ((index1 >> 2) & 7);
		int address = 0x3C0 | pos;
		int colorByte = read((index1 & 0x2C00) | address);
		/* figure out which bits in the color table to use */
		int colorBits = ((index1 >> 4) & 4) + (index1 & 2);
		// page2 is the output of the nametable read (this section is the FIRST read per tile!)
		int page2 = read(index1);
		if (startX < VisibleScreenWidth) {
			int color = ((colorByte >> colorBits) & 3) * 4;
			// need to read 0x0000 or 0x1000 + 16*nametable data
			address = m_tilePageOffset | (page2 * 16);
			// plus something that accounts for y
			address |= scrollTileYOffset;

			quint8 plane1 = read(address & 0x1FFF);
			quint8 plane2 = read((address + 8) & 0x1FFF);
			/* render the pixel */
			int endX = startX + 8;
			for (; startX < endX; startX++) {
				//	TODO flatten loop
				quint8 pix = ((plane1 >> 7) & 1) | (((plane2 >> 7) & 1) << 1);
				plane1 <<= 1;
				plane2 <<= 1;
				if (startX >= 0 && startX < VisibleScreenWidth) {
					if (pix) {
						*dest = m_palette->penRgb(color | pix);
						m_linePriority[startX] |= 2;
					} else
						*dest = backPen;
				}
				dest++;
			}
			/* move to next tile over and toggle the horizontal name table if necessary */
			x++;
			if (x > 31) {
				x = 0;
				tileIndex ^= 0x400;
			}
		}
	}
//	int startX = 8 - m_scrollTileXOffset;
//	QRgb *pScn = reinterpret_cast<QRgb *> (m_frame.scanLine(m_scanline)) + startX;
//	int *linePriority = m_linePriority;

//	int	ntbladr = 0x2000+(m_refreshData&0x0FFF);
//	int	attradr = 0x03C0+((m_refreshData&0x0380)>>4);
//	int	ntbl_x  = ntbladr&0x001F;
//	int	attrsft = (ntbladr&0x0040)>>4;

//	int	tileadr;
//	int	cache_tile = 0xFFFF0000;
//	quint8	cache_attr = 0xFF;
//	quint8	chr_h, chr_l, attr;

//	for(int i = 0; i < 33; i++) {
//		tileadr = m_tilePageOffset+READ(ntbladr)*0x10+scrollTileYOffset;
//		attr = ((READ(0x2000 + (ntbladr & 0xC00) + attradr+(ntbl_x>>2))>>((ntbl_x&2)+attrsft))&3)<<2;

//		if( cache_tile != tileadr || cache_attr != attr ) {
//			cache_tile = tileadr;
//			cache_attr = attr;

//			chr_l = READ(tileadr);
//			chr_h = READ(tileadr+8);
//			//*pBGw = chr_l|chr_h;

//			int	c1 = ((chr_l>>1)&0x55)|(chr_h&0xAA);
//			int	c2 = (chr_l&0x55)|((chr_h<<1)&0xAA);
//			int c = attr | (c1>>6);
//			if (c & 3)
//				linePriority[0] |= 2;
//			pScn[0] = m_palette->penRgb(c);
//			c = attr | ((c1>>2)&3);
//			if (c & 3)
//				linePriority[4] |= 2;
//			pScn[4] = m_palette->penRgb(c);
//			c = attr | (c2>>6);
//			if (c & 3)
//				linePriority[1] |= 2;
//			pScn[1] = m_palette->penRgb(c);
//			c = attr | ((c2>>2) & 3);
//			if (c & 3)
//				linePriority[5] |= 2;
//			pScn[5] = m_palette->penRgb(c);
//			c = attr | ((c1>>4)&3);
//			if (c & 3)
//				linePriority[2] |= 2;
//			pScn[2] = m_palette->penRgb(c);
//			c = attr | (c1&3);
//			if (c & 3)
//				linePriority[6] |= 2;
//			pScn[6] = m_palette->penRgb(c);
//			c = attr | ((c2>>4)&3);
//			if (c & 3)
//				linePriority[3] |= 2;
//			pScn[3] = m_palette->penRgb(c);
//			c = attr | (c2&3);
//			if (c & 3)
//				linePriority[7] |= 2;
//			pScn[7] = m_palette->penRgb(c);
//		} else {
//			for (int j = 0; j < 8; j++) {
//				pScn[j] = pScn[j - 8];
//				linePriority[j] = linePriority[j - 8];
//			}
//		}
//		pScn+=8;
//		linePriority += 8;
//		if( ++ntbl_x == 32 ) {
//			ntbl_x = 0;
//			ntbladr ^= 0x41F;
//			attradr = 0x03C0+((ntbladr&0x0380)>>4);
//		} else {
//			ntbladr++;
//		}
//	}

	/* if the left 8 pixels for the background are off, blank 'em */
	if (m_registers->isBackgroundClippingEnabled()) {
		fillScanline(0, 8);
		for (int i = 0; i < 8; i++)
			m_linePriority[i] ^= 2;
	}
}

void Nes2C0XPpu::drawSprites() {
	int count = 0;
	int size = m_registers->spriteSize();
	int firstPixel = m_registers->isSpriteClippingEnabled() ? 8 : 0;
	for (int spriteIndex = 0; spriteIndex < NumSprites; spriteIndex++) {
		Nes2C0XSprite *sprite = reinterpret_cast<Nes2C0XSprite *> (m_spriteMemory + spriteIndex * 4);
		// The sprite collision acts funny on the last pixel of a scanline.
		// The various scanline latches update while the last few pixels
		// are being drawn. Since we don't do cycle-by-cycle PPU emulation,
		// we fudge it a bit here so that sprite 0 collisions are detected
		// when, e.g., sprite x is 254, sprite y is 29 and we're rendering
		// at the end of scanline 28.
		// Battletoads needs this level of precision to be playable.
		if ((spriteIndex == 0) && (sprite->x() == 254)) {
			sprite->m_y--;
			/* set the "sprite 0 hit" flag if appropriate */
			if (m_linePriority[sprite->x()] & 2)
				m_registers->setSprite0Hit();
		}
		/* if the sprite isn't visible, skip it */
		if ((sprite->y() + size <= m_scanline) || (sprite->y() > m_scanline))
			continue;
		int tile = sprite->tileIndex();
		if (size == 16 && (tile & 1)) {
			/* if it's 8x16 and odd-numbered, draw the other half instead */
			tile &= ~1;
			tile |= 0x100;
		}
		/* compute the character's line to draw */
		int spriteLine = m_scanline - sprite->y();
		if (sprite->flipVertically())
			spriteLine = (size - 1) - spriteLine;
		if (size == 16 && spriteLine > 7) {
			tile++;
			spriteLine -= 8;
		}
		int index1 = tile * 16;
		if (size == 8)
			index1 |= m_spritePageOffset;

		quint8 plane1 = read((index1 | (spriteLine + 0)) & 0x1FFF);
		quint8 plane2 = read((index1 | (spriteLine + 8)) & 0x1FFF);
		/* if there are more than 8 sprites on this line, set the flag and abort drawing sprites */
		if (count == 8) {
			m_registers->setSpriteMax();
			break;
		}
		count++;
		/* abort drawing if sprites aren't rendered */
		if (!m_registers->isSpriteVisible())
			continue;
		for (int pixel = 0; pixel < 8; pixel++) {
			quint8 pixelData;
			if (sprite->flipHorizontally()) {
				pixelData = (plane1 & 1) + ((plane2 & 1) << 1);
				plane1 >>= 1;
				plane2 >>= 1;
			} else {
				pixelData = ((plane1 >> 7) & 1) | (((plane2 >> 7) & 1) << 1);
				plane1 <<= 1;
				plane2 <<= 1;
			}
			/* is this pixel non-transparent? */
			if (sprite->x() + pixel >= firstPixel) {
				if ((pixelData & 3) && (sprite->x() + pixel) < VisibleScreenWidth) {
					int mask = (sprite->isBehindBackground() ? 3 : 1);
					if (!(m_linePriority[sprite->x() + pixel] & mask)) {
						int color = sprite->paletteHighBits() | pixelData | 0x10;
						m_frame.setPixel(sprite->x() + pixel, m_scanline, m_palette->penRgb(color));
					}
					m_linePriority[sprite->x() + pixel] |= 1;
				}
				/* set the "sprite 0 hit" flag if appropriate */
				if (spriteIndex == 0 && (pixelData & 3) && (sprite->x() + pixel < 255)
						&& (m_linePriority[sprite->x() + pixel] & 2)) {
					m_registers->setSprite0Hit();
				}
			}
		}
	}
}

void Nes2C0XPpu::renderScanline() {
	/* clear the line priority for this scanline */
	qMemSet(m_linePriority, 0, sizeof(m_linePriority));
	drawBackground();
	drawSprites();
}

void Nes2C0XPpu::updateScanline() {
	if (m_scanline < VisibleScreenHeight) {
		/* Render this scanline if appropriate */
		if (m_registers->isDisplayOn()) {
			/* If background or sprites are enabled, copy the ppu address latch */
			/* Copy only the scroll x-coarse and the x-overflow bit */
			m_refreshData &= ~0x041F;
			m_refreshData |= (m_refreshLatch & 0x041F);
			renderScanline();
		} else {
			// If the PPU's VRAM address happens to point into palette ram space while
			// both the sprites and background are disabled, the PPU paints the scanline
			// with the palette entry at the VRAM address instead of the usual background
			// pen. Micro Machines makes use of this feature.
			if ((m_vramAddress & PalettesAddress) == PalettesAddress)
				fillScanline(m_vramAddress & 0x1F);
			else
				fillScanline(0);
		}
		/* increment the fine y-scroll */
		m_refreshData += 0x1000;
		/* if it's rolled, increment the coarse y-scroll */
		if (m_refreshData & 0x8000) {
			quint16 tmp = (m_refreshData & 0x03E0) + 0x20;
			m_refreshData &= 0x7C1F;
			/* handle bizarro scrolling rollover at the 30th (not 32nd) vertical tile */
			if (tmp == 0x03C0)
				m_refreshData ^= 0x0800;
			else
				m_refreshData |= (tmp & 0x03E0);
		}
	}
}

void Nes2C0XPpu::processScanline(bool *lastLine) {
	*lastLine = false;
	/* update the scanline that just went by */
	updateScanline();
	/* increment our scanline count */
	m_scanline++;
	/* Note: this is called at the _end_ of each scanline */
	if (m_scanline == VBlankFirstScanline)
		m_registers->setVBlank();
	if (m_scanline == m_scanlinesPerFrame - 1)
		m_registers->clearStatus();
	else if (m_scanline == m_scanlinesPerFrame) { /* if we rolled */
		/* if background or sprites are enabled, copy the ppu address latch */
		if (m_registers->isDisplayOn())
			m_refreshData = m_refreshLatch;
		m_scanline = 0;
		*lastLine = true;
	}
}

void Nes2C0XPpu::fillScanline(int color, int count) {
	QRgb pen = m_palette->penRgb(color);
	QRgb *data = reinterpret_cast<QRgb *> (m_frame.scanLine(m_scanline));
	for (int i = 0; i < count; i++)
		data[i] = pen;
}
