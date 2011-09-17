#ifndef NESPPU_H
#define NESPPU_H

class NesPpu;
#include <imachine.h>
#include <QImage>
#include <QVector>
#include <QRgb>

class NesPpu;

extern QImage nesPpuFrame;
extern NesPpu nesPpu;
extern u16 nesPpuTilePageOffset;
extern u8 nesPpuScrollTileYOffset;
extern u8 nesPpuRegs[4];
extern int nesPpuScanline;
extern int nesPpuScanlinesPerFrame;

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

class NesPpu : public QObject {
	Q_OBJECT
	Q_ENUMS(RenderMethod)
	Q_PROPERTY(RenderMethod renderMethod READ renderMethod WRITE setRenderMethod NOTIFY renderMethodChanged)
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

	static const int ScanlinesPerFrameNTSC = 262;
	static const int ScanlinesPerFramePAL = 312;

	static const int NameTableOffset = 0x2000;
	static const int AttributeTableOffset = 0x03C0;
	static const int PalettesAddress = 0x3F00;

	static const int VisibleScreenWidth = 32 * 8;
	static const int VisibleScreenHeight = 30 * 8;

	static const int FetchCycles = 8;

	void init();

	void writeReg(u16 address, u8 data);
	u8 readReg(u16 address);

	void setChipType(ChipType newType);

	RenderMethod renderMethod() const;
	void setRenderMethod(RenderMethod method);

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

	bool save(QDataStream &s);
	bool load(QDataStream &s);
signals:
	void renderMethodChanged();
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

#endif // NESPPU_H
