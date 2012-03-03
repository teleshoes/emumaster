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

#ifndef NESPPU_H
#define NESPPU_H

class NesPpu;
#include <emu.h>
#include <QImage>
#include <QVector>
#include <QRgb>
#include <QStringList>

class NesPpu;
class NesPpuScroll;

extern QImage nesPpuFrame;
extern NesPpu nesPpu;
extern NesPpuScroll nesPpuScroll;
extern u16 nesPpuTilePageOffset;
extern u8 nesPpuRegs[8];
extern int nesPpuScanline;

class NesPpuSprite {
public:
	enum AttributeBit {
		FlipVertically		= 0x80,
		FlipHorizontally	= 0x40,
		BehindBackground	= 0x20,
		HighPaletteBitsMask	= 0x03
	};
	int x() const;
	int y() const;
	u8 tileIndex() const;
	u8 paletteHighBits() const;
	bool flipHorizontally() const;
	bool flipVertically() const;
	bool isBehindBackground() const;
private:
	u8 m_y;
	u8 m_tileIndex;
	u8 m_attributes;
	u8 m_x;

	friend class Nes2C0XPpu;
} Q_PACKED;

inline int NesPpuSprite::x() const
{ return m_x; }
inline int NesPpuSprite::y() const
{ return m_y + 1; }
inline u8 NesPpuSprite::tileIndex() const
{ return m_tileIndex; }
inline u8 NesPpuSprite::paletteHighBits() const
{ return (m_attributes & HighPaletteBitsMask) << 2; }
inline bool NesPpuSprite::flipHorizontally() const
{ return m_attributes & FlipHorizontally; }
inline bool NesPpuSprite::flipVertically() const
{ return m_attributes & FlipVertically; }
inline bool NesPpuSprite::isBehindBackground() const
{ return m_attributes & BehindBackground; }

class NesPpu : public QObject
{
	Q_OBJECT
public:
	enum ChipType {
		PPU2C02,	// NTSC NES
		PPU2C03B,	// Playchoice 10
		PPU2C04,	// Vs. Unisystem
		PPU2C05_01,	// Vs. Unisystem (Ninja Jajamaru Kun)
		PPU2C05_02,	// Vs. Unisystem (Mighty Bomb Jack)
		PPU2C05_03,	// Vs. Unisystem (Gumshoe)
		PPU2C05_04,	// Vs. Unisystem (Top Gun)
		PPU2C07		// PAL NES
	};
	enum Register {
		Control0 = 0,
		Control1 = 1,
		Status = 2,
		SpriteRAMAddress = 3,
		SpriteRAMIO = 4,
		Scroll = 5,
		VRAMAddress = 6,
		VRAMIO = 7
	};
	enum ControlReg0Bit {
		//	Indicates whether a NMI should occur upon V-Blank.
		VBlankEnableCR0Bit = 0x80,
		//	Specifies the size of sprites in pixels 8x8 (0) or 8x16(1).
		SpriteSizeCR0Bit = 0x20,
		/* Identifies which pattern table the background is
		 * stored in, either $0000 (0) or $1000 (1). */
		BackgroundTableCR0Bit = 0x10,
		/* Identifies which pattern table sprites are stored in,
		 * either $0000 (0) or $1000 (1).*/
		SpriteTableCR0Bit = 0x08,
		/* Specifies amount to increment address by, either 1 if
		 * this is 0 or 32 if this is 1. */
		IncrementCR0Bit = 0x04,
		/* Name table address, changes between the four
		 * name tables at $2000 (0), $2400 (1), $2800 (2) and $2C00 (3). */
		NameTableAddressMaskCR0Bit = 3
	};
	Q_DECLARE_FLAGS(ControlReg0, ControlReg0Bit)

	enum ControlReg1Bit {
		/* Indicates background colour in monochrome
		 * mode or colour intensity in colour mode. */
		BackgroundColorCR1Bit = 0xE0,
		//	If this is 0, sprites should not be displayed.
		SpriteDisplayCR1Bit = 0x10,
		//	If this is 0, the background should not be displayed.
		BackgroundDisplayCR1Bit = 0x08,
		/* Specifies whether to clip the sprites, that is whether
		 * to hide sprites in the left 8 pixels on screen (0) or to show
		 * them (1). */
		SpriteClipDisableCR1Bit = 0x04,
		/* Specifies whether to clip the background, that is
		 * whether to hide the background in the left 8 pixels on
		 * screen (0) or to show them (1). */
		BackgroundClipDisableCR1Bit = 0x02,
		/* Indicates whether the system is in colour (0) or
		 * monochrome mode (1) */
		MonochromeModeCR1Bit = 0x01
	};
	Q_DECLARE_FLAGS(ControlReg1, ControlReg1Bit)

	enum StatusRegBit {
		//	Indicates whether V-Blank is occurring.
		VBlankSRBit = 0x80,
		/* Sprite 0 hit flag, set when a non-transparent pixel of
		 * sprite 0 overlaps a non-transparent background pixel. */
		Sprite0HitSRBit = 0x40,
		/* Scanline sprite count, if set, indicates more than 8
		 * sprites on the current scanline. */
		SpriteMaxSRBit = 0x20,
		//	If set, indicates that writes to VRAM should be ignored.
		DisableVRAMWriteSRBit = 0x10
	};
	Q_DECLARE_FLAGS(StatusReg, StatusRegBit)

	static const uint NameTableOffset = 0x2000;
	static const uint AttributeTableOffset = 0x03C0;
	static const uint PalettesAddress = 0x3F00;

	static const int VisibleScreenWidth = 32 * 8;
	static const int VisibleScreenHeight = 30 * 8;

	static const int FetchCycles = 8;

	void init();

	void writeReg(u16 addr, u8 data);
	u8 readReg(u16 address);

	void setChipType(ChipType newType);

	bool isBackgroundVisible() const;
	bool isSpriteVisible() const;
	bool isDisplayOn() const;

	void nextScanline();

	void setCharacterLatchEnabled(bool on);
	void setExternalLatchEnabled(bool on);

	void setVBlank(bool on);
	void dma(u8 page);

	void processFrameStart();
	void processScanlineStart();
	void processScanlineNext();
	void processScanline();
	void processDummyScanline();
	bool checkSprite0HitHere() const;

	void sl();
private:
	void drawBackground();
	void drawBackgroundNoTileNoExtLatch();
	void drawBackgroundTileNoExtLatch();
	void drawBackgroundNoTileExtLatch();
	void drawBackgroundTileExtLatch();

	void drawSprites();
	void fillScanline(int color, int count);
};

inline bool NesPpu::isBackgroundVisible() const
{ return nesPpuRegs[Control1] & BackgroundDisplayCR1Bit; }
inline bool NesPpu::isSpriteVisible() const
{ return nesPpuRegs[Control1] & SpriteDisplayCR1Bit; }
inline bool NesPpu::isDisplayOn() const
{ return isBackgroundVisible() || isSpriteVisible(); }

class NesPpuScroll
{
public:
	enum {
		X_TILE    = 0x001F,
		Y_TILE    = 0x03E0,
		Y_FINE    = 0x7000,
		LOW       = 0x00FF,
		HIGH      = 0xFF00,
		NAME      = 0x0C00,
		NAME_LOW  = 0x0400,
		NAME_HIGH = 0x0800
	};

	void clockX();
	void resetX();
	void clockY();
	uint yFine();

	uint address;
	uint toggle;
	uint latch;
	uint xFine;
};

inline void NesPpuScroll::clockX()
{
	if ((address & X_TILE) != X_TILE)
		address++;
	else
		address ^= (X_TILE|NAME_LOW);
}

inline void NesPpuScroll::resetX()
{
	address = (address & ((X_TILE|NAME_LOW) ^ 0x7FFF)) | (latch & (X_TILE|NAME_LOW));
}

inline void NesPpuScroll::clockY()
{
	if ((address & Y_FINE) != Y_FINE) {
		address += 1 << 12;
	} else switch (address & Y_TILE) {
		default:         address = (address & (Y_FINE ^ 0x7FFF)) + (1 << 5); break;
		case (29 << 5): address ^= NAME_HIGH;
		case (31 << 5): address &= (Y_FINE|Y_TILE) ^ 0x7FFF; break;
	}
}

inline uint NesPpuScroll::yFine()
{
	return address >> 12;
}

QRgb nesPpuGetPixel(int x, int y);

#endif // NESPPU_H
