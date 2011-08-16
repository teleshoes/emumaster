#ifndef NESPPUREGISTERS_H
#define NESPPUREGISTERS_H

class NesPpu;
#include "nes_global.h"
#include <QObject>

class NES_EXPORT NesPpuRegisters : public QObject {
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

	explicit NesPpuRegisters(NesPpu *ppu = 0);
	void updateType();

	void write(quint16 address, quint8 data);
	quint8 read(quint16 address);

	bool isBackgroundVisible() const;
	bool isSpriteVisible() const;
	bool isDisplayOn() const;

	bool isBackgroundClippingEnabled() const;
	bool isSpriteClippingEnabled() const;

	int spriteSize() const;

	void setSprite0Hit();
	bool sprite0HitOccurred() const;

	void setSpriteMax(bool on);

	void setVBlank(bool on);
	bool isVBlank() const;
	bool isVBlankEnabled() const;

	int colorEmphasis() const;
	bool isMonochromeModeSet() const;

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	quint8 m_regs[4]; // registers at 0x2000-0x2003
	bool m_toggle;
	quint8 m_dataLatch;
	quint16 m_add;
	quint8 m_bufferedData;
	quint8 m_securityValue;
};

inline bool NesPpuRegisters::isBackgroundVisible() const
{ return m_regs[Control1] & BackgroundDisplayCR1Bit; }
inline bool NesPpuRegisters::isSpriteVisible() const
{ return m_regs[Control1] & SpriteDisplayCR1Bit; }
inline bool NesPpuRegisters::isDisplayOn() const
{ return isBackgroundVisible() || isSpriteVisible(); }
inline bool NesPpuRegisters::isBackgroundClippingEnabled() const
{ return !(m_regs[Control1] & BackgroundClipDisableCR1Bit); }
inline bool NesPpuRegisters::isSpriteClippingEnabled() const
{ return !(m_regs[Control1] & SpriteClipDisableCR1Bit); }
inline int NesPpuRegisters::spriteSize() const
{ return (m_regs[Control0] & SpriteSizeCR0Bit) ? 16 : 8; }
inline void NesPpuRegisters::setSprite0Hit()
{ m_regs[Status] |= Sprite0HitSRBit; }
inline bool NesPpuRegisters::sprite0HitOccurred() const
{ return m_regs[Status] & Sprite0HitSRBit; }
inline void NesPpuRegisters::setSpriteMax(bool on)
{ if (on) m_regs[Status] |= SpriteMaxSRBit; else m_regs[Status] &= ~SpriteMaxSRBit; }
inline bool NesPpuRegisters::isVBlank() const
{ return m_regs[Status] & VBlankSRBit; }
inline bool NesPpuRegisters::isVBlankEnabled() const
{ return m_regs[Control0] & VBlankEnableCR0Bit; }
inline int NesPpuRegisters::colorEmphasis() const
{ return static_cast<int>(m_regs[Control1] & BackgroundColorCR1Bit) * 2; }
inline bool NesPpuRegisters::isMonochromeModeSet() const
{ return m_regs[Control1] & MonochromeModeCR1Bit; }

#endif // NESPPUREGISTERS_H
