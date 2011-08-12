#ifndef NES2C0XPPU_H
#define NES2C0XPPU_H

#include "registers.h"
#include "palette.h"
#include <QString>
#include <QImage>

class NES2C0X_EXPORT Nes2C0XPpu : public QObject {
	Q_OBJECT
public:
	enum ChipType {
		PPU2C02 = 0,// NTSC NES
		PPU2C03B,	// Playchoice 10
		PPU2C04,	// Vs. Unisystem
		PPU2C05_01,	// Vs. Unisystem (Ninja Jajamaru Kun)
		PPU2C05_02,	// Vs. Unisystem (Mighty Bomb Jack)
		PPU2C05_03,	// Vs. Unisystem (Gumshoe)
		PPU2C05_04,	// Vs. Unisystem (Top Gun)
		PPU2C07		// PAL NES
	};
	static const int ScanlinesPerFrameNTSC = 262;
	static const int ScanlinesPerFramePAL = 312;
	static const int VBlankFirstScanline = 241;
	static const int VBlankLastScanlineNTSC = 260;
	static const int VBlankLastScanlinePAL = 310;

	static const int PatternTable1Address = 0x1000;
	static const int NameTablesAddress = 0x2000;
	static const int AttributeTableOffset = 0x03C0;
	static const int PalettesAddress = 0x3F00;

	static const int NumSprites = 64;
	static const int SpriteMemorySize = NumSprites * 4;

	explicit Nes2C0XPpu(QObject *parent = 0);

	ChipType chipType() const;
	void setChipType(ChipType newType);

	Nes2C0XRegisters *registers() const;
	Nes2C0XPalette *palette() const;

	void processScanline(bool *lastLine);
	int scanline() const;
	const QImage &frame() const;
protected:
	virtual void write(quint16 address, quint8 data) = 0;
	virtual quint8 read(quint16 address) = 0;
signals:
	void vblank_o(bool on);
private:
	static const int BlackColorForClipping = 64;
	static const int VisibleScreenWidth = 32 * 8;
	static const int VisibleScreenHeight = 30 * 8;

	void drawBackground();
	void drawSprites();
	void renderScanline();
	void updateScanline();
	void fillScanline(int color, int count = VisibleScreenWidth);
	void updateVBlankOut();

	Nes2C0XRegisters *m_registers;
	Nes2C0XPalette *m_palette;
	quint8 m_spriteMemory[0x100];

	ChipType m_type;
	int m_scanlinesPerFrame;

	int m_scanline;
	quint16 m_vramAddress;
	quint16 m_refreshData;
	quint16 m_refreshLatch;
	quint8 m_scrollTileXOffset;
	quint16 m_tilePageOffset;
	quint16 m_spritePageOffset;
	bool m_vBlankOut;

	int m_linePriority[VisibleScreenWidth];
	QImage m_frame;

	friend class Nes2C0XRegisters;
	friend class Nes2C0XPalette;
};

inline Nes2C0XRegisters *Nes2C0XPpu::registers() const
{ return m_registers; }
inline Nes2C0XPalette *Nes2C0XPpu::palette() const
{ return m_palette; }
inline Nes2C0XPpu::ChipType Nes2C0XPpu::chipType() const
{ return m_type; }
inline int Nes2C0XPpu::scanline() const
{ return m_scanline; }
inline const QImage &Nes2C0XPpu::frame() const
{ return m_frame; }

#endif // NES2C0XPPU_H
