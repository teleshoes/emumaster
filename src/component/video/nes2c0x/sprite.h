#ifndef NES2C0XSPRITE_H
#define NES2C0XSPRITE_H

#include "nes2c0x_global.h"

class NES2C0X_EXPORT Nes2C0XSprite {
public:
	enum AttributeBit {
		FlipVertically		= 0x80,
		FlipHorizontally	= 0x40,
		BehindBackground	= 0x20,
		HighPaletteBitsMask	= 0x03
	};
	int x() const;
	int y() const;
	quint8 tileIndex() const;
	quint8 paletteHighBits() const;
	bool flipHorizontally() const;
	bool flipVertically() const;
	bool isBehindBackground() const;
private:
	quint8 m_y;
	quint8 m_tileIndex;
	quint8 m_attributes;
	quint8 m_x;

	friend class Nes2C0XPpu;
};

inline int Nes2C0XSprite::x() const
{ return m_x; }
inline int Nes2C0XSprite::y() const
{ return m_y + 1; }
inline quint8 Nes2C0XSprite::tileIndex() const
{ return m_tileIndex; }
inline quint8 Nes2C0XSprite::paletteHighBits() const
{ return (m_attributes & HighPaletteBitsMask) << 2; }
inline bool Nes2C0XSprite::flipHorizontally() const
{ return m_attributes & FlipHorizontally; }
inline bool Nes2C0XSprite::flipVertically() const
{ return m_attributes & FlipVertically; }
inline bool Nes2C0XSprite::isBehindBackground() const
{ return m_attributes & BehindBackground; }

#endif // NES2C0XSPRITE_H
