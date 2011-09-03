#ifndef NESSPRITE_H
#define NESSPRITE_H

#include "nes_global.h"

class NES_EXPORT NesPpuSprite {
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
} Q_PACKED;

inline int NesPpuSprite::x() const
{ return m_x; }
inline int NesPpuSprite::y() const
{ return m_y + 1; }
inline quint8 NesPpuSprite::tileIndex() const
{ return m_tileIndex; }
inline quint8 NesPpuSprite::paletteHighBits() const
{ return (m_attributes & HighPaletteBitsMask) << 2; }
inline bool NesPpuSprite::flipHorizontally() const
{ return m_attributes & FlipHorizontally; }
inline bool NesPpuSprite::flipVertically() const
{ return m_attributes & FlipVertically; }
inline bool NesPpuSprite::isBehindBackground() const
{ return m_attributes & BehindBackground; }

#endif // NESSPRITE_H
