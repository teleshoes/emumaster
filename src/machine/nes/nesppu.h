#ifndef NESPPU_H
#define NESPPU_H

class NesPpuMapper;
class NesCpuMapper;
class NesMachine;
#include "nesppuregisters.h"
#include "nesppupalette.h"
#include <QImage>

class NES_EXPORT NesPpu : public QObject {
	Q_OBJECT
	Q_ENUMS(RenderMethod)
	Q_PROPERTY(RenderMethod renderMethod READ renderMethod WRITE setRenderMethod NOTIFY renderMethodChanged)
	Q_PROPERTY(bool spriteClippingEnable READ isSpriteClippingEnabled WRITE setSpriteClippingEnabled NOTIFY spriteClippingEnableChanged)
public:
	enum ChipType {
		PPU2C02 = 0,// NTSC NES
		PPU2C03B,	// Playchoice 10
		PPU2C04,	// Vs. Unisystem // TODO test
		PPU2C05_01,	// Vs. Unisystem (Ninja Jajamaru Kun) // TODO test
		PPU2C05_02,	// Vs. Unisystem (Mighty Bomb Jack) // TODO test
		PPU2C05_03,	// Vs. Unisystem (Gumshoe) // TODO test
		PPU2C05_04,	// Vs. Unisystem (Top Gun) // TODO test
		PPU2C07		// PAL NES
	};
	enum RenderMethod {
		PostAllRender = 0,
		PreAllRender,
		PostRender,
		PreRender,
		TileRender
	};

	static const int ScanlinesPerFrameNTSC = 262;
	static const int ScanlinesPerFramePAL = 312;

	static const int NameTableOffset = 0x2000;
	static const int AttributeTableOffset = 0x03C0;
	static const int PalettesAddress = 0x3F00;

	static const int NumSprites = 64;

	static const int VisibleScreenWidth = 32 * 8;
	static const int VisibleScreenHeight = 30 * 8;

	static const int FetchCycles = 8;

	explicit NesPpu(NesMachine *machine);
	~NesPpu();

	NesMachine *machine() const;

	void setMapper(NesPpuMapper *mapper);

	NesPpuRegisters *registers() const;
	NesPpuPalette *palette() const;

	ChipType chipType() const;
	void setChipType(ChipType newType);

	int scanlinesCount() const;

	RenderMethod renderMethod() const;
	void setRenderMethod(RenderMethod method);

	int scanline() const;
	void setScanline(int line);

	void setCharacterLatchEnabled(bool on);
	void setExternalLatchEnabled(bool on);

	void setVBlank(bool on);
	void dma(NesCpuMapper *cpuMapper, quint8 page);

	void processFrameStart();
	void processFrameEnd();
	void processScanlineStart();
	void processScanlineNext();
	void processScanline();
	void processDummyScanline();
	bool checkSprite0HitHere() const;

	const QImage &frame() const;

	bool save(QDataStream &s);
	bool load(QDataStream &s);

	quint8 scrollTileYOffset() const;
	quint16 tilePageOffset() const;

	bool isSpriteClippingEnabled() const;
	void setSpriteClippingEnabled(bool on);
signals:
	void vblank_o(bool on);
	void renderMethodChanged();
	void spriteClippingEnableChanged();
private:
	void drawBackground();
	void drawBackgroundNoTileNoExtLatch();
	void drawBackgroundTileNoExtLatch();
	void drawBackgroundNoTileExtLatch();
	void drawBackgroundTileExtLatch();

	void drawSprites();
	void fillScanline(int color, int count);

	void updateVBlankOut();

	NesPpuRegisters *m_registers;
	NesPpuPalette *m_palette;

	NesPpuMapper *m_mapper;

	ChipType m_type;
	RenderMethod m_renderMethod;

	int m_scanline;
	int m_scanlinesPerFrame;
	QRgb *m_scanlineData;
	QRgb *m_scanline0Data;
	QImage m_frame;

	bool m_characterLatchEnabled;
	bool m_externalLatchEnabled;

	bool m_vBlankOut;

	quint16 m_vramAddress;
	quint16 m_refreshLatch;
	quint8 m_scrollTileXOffset;
	quint8 m_scrollTileYOffset;
	quint16 m_tilePageOffset;
	quint16 m_spritePageOffset;
	quint16 m_loopyShift;
	quint8 m_bgWritten[33];
	quint8 m_bit2Rev[256];

	quint8 m_spriteMemory[NumSprites*4];

	bool m_spriteClippingEnable;

	friend class NesPpuRegisters;
	friend class NesPpuPalette;
};

inline NesPpuRegisters *NesPpu::registers() const
{ return m_registers; }
inline NesPpuPalette *NesPpu::palette() const
{ return m_palette; }

inline NesPpu::ChipType NesPpu::chipType() const
{ return m_type; }
inline int NesPpu::scanlinesCount() const
{ return m_scanlinesPerFrame; }
inline NesPpu::RenderMethod NesPpu::renderMethod() const
{ return m_renderMethod; }

inline int NesPpu::scanline() const
{ return m_scanline; }
inline const QImage &NesPpu::frame() const
{ return m_frame; }

inline void NesPpu::setVBlank(bool on)
{ m_registers->setVBlank(on); }

inline quint8 NesPpu::scrollTileYOffset() const
{ return m_scrollTileYOffset; }
inline quint16 NesPpu::tilePageOffset() const
{ return m_tilePageOffset; }

inline bool NesPpu::isSpriteClippingEnabled() const
{ return m_spriteClippingEnable; }

#endif // NESPPU_H
