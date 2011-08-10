#ifndef NES2C0XPALETTE_H
#define NES2C0XPALETTE_H

#include "nes2c0x_global.h"
#include <QObject>
#include <QVector>
#include <QRgb>
class Nes2C0XPpu;

class NES2C0X_EXPORT Nes2C0XPalette : public QObject {
	Q_OBJECT
public:
	explicit Nes2C0XPalette(Nes2C0XPpu *ppu = 0);
	void write(quint16 address, quint8 data);
	quint8 read(quint16 address) const;
	void update();
	QRgb penRgb(int i) const;
private:
	void fill();
	int colorEmphasis() const;
	int colorMask() const;

	quint8 m_memory[32];
	QRgb m_pens[512];
	int m_mask;
	int m_emphasis;

	static const quint8 defaultMemory[];

	friend class Nes2C0XPpu;
};

inline QRgb Nes2C0XPalette::penRgb(int i) const
{ return m_pens[(m_memory[i] & m_mask) + m_emphasis]; }

#endif // NES2C0XPALETTE_H
