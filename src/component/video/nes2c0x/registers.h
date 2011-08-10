#ifndef NES2C0XREGISTERS_H
#define NES2C0XREGISTERS_H

class Nes2C0XPpu;
#include "nes2c0x_global.h"
#include <QObject>

class NES2C0X_EXPORT Nes2C0XRegisters : public QObject {
	Q_OBJECT
public:
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
		//	TODO MasterSlaveCR0Bit = 0x40, ???
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
		DisplaySpriteCR1Bit = 0x10,
		//	If this is 0, the background should not be displayed.
		DisplayBackgroundCR1Bit = 0x08,
		/* Specifies whether to clip the sprites, that is whether
		 * to hide sprites in the left 8 pixels on screen (0) or to show
		 * them (1). */
		DisableSpriteClipCR1Bit = 0x04,
		/* Specifies whether to clip the background, that is
		 * whether to hide the background in the left 8 pixels on
		 * screen (0) or to show them (1). */
		DisableBackgroundClipCR1Bit = 0x02,
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

	explicit Nes2C0XRegisters(Nes2C0XPpu *ppu = 0);
	void write(quint16 address, quint8 data);
	quint8 read(quint16 address);
	void updateType();
	bool isBackgroundVisible() const;
	bool isSpriteVisible() const;
	bool isBackgroundClippingEnabled() const;
	bool isSpriteClippingEnabled() const;
	int spriteSize() const;
	void setSprite0Hit();
	void setSpriteMax();
	void setVBlank();
	bool isVBlank() const;
	bool isVBlankEnabled() const;
	void clearStatus();
	int colorEmphasis() const;
	bool isMonochromeModeSet() const;
private:
	quint8 m_regs[8];
	bool m_toggle;
	quint8 m_dataLatch;
	quint16 m_add;
	quint8 m_bufferedData;
	quint8 m_securityValue;
};

inline bool Nes2C0XRegisters::isBackgroundVisible() const
{ return m_regs[Control1] & DisplayBackgroundCR1Bit; }
inline bool Nes2C0XRegisters::isSpriteVisible() const
{ return m_regs[Control1] & DisplaySpriteCR1Bit; }
inline bool Nes2C0XRegisters::isBackgroundClippingEnabled() const
{ return !(m_regs[Control1] & DisableBackgroundClipCR1Bit); }
inline bool Nes2C0XRegisters::isSpriteClippingEnabled() const
{ return !(m_regs[Control1] & DisableSpriteClipCR1Bit); }
inline int Nes2C0XRegisters::spriteSize() const
{ return (m_regs[Control0] & SpriteSizeCR0Bit) ? 16 : 8; }
inline void Nes2C0XRegisters::setSprite0Hit()
{ m_regs[Status] |= Sprite0HitSRBit; }
inline void Nes2C0XRegisters::setSpriteMax()
{ m_regs[Status] |= SpriteMaxSRBit; }
inline bool Nes2C0XRegisters::isVBlank() const
{ return m_regs[Status] & VBlankSRBit; }
inline bool Nes2C0XRegisters::isVBlankEnabled() const
{ return m_regs[Control0] & VBlankEnableCR0Bit; }
inline int Nes2C0XRegisters::colorEmphasis() const
{ return static_cast<int>(m_regs[Control1] & BackgroundColorCR1Bit) * 2; }
inline bool Nes2C0XRegisters::isMonochromeModeSet() const
{ return m_regs[Control1] & MonochromeModeCR1Bit; }

#endif // NES2C0XREGISTERS_H
