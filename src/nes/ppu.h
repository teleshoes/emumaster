#ifndef NESPPU_H
#define NESPPU_H

class NesPpu;
#include <imachine.h>
#include <QImage>
#include <QVector>
#include <QRgb>

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

class NesPpuPalette : public QObject {
	Q_OBJECT
public:
	void init();
	void write(u16 address, u8 data);
	u8 read(u16 address) const;
	void updateColorEmphasisAndMask();

	QRgb *currentPens();

	bool save(QDataStream &s);
	bool load(QDataStream &s);
};

class NesPpuRegisters : public QObject {
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

	void write(u16 address, u8 data);
	u8 read(u16 address);

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
	u8 m_regs[4]; // registers at 0x2000-0x2003
	bool m_toggle;
	u8 m_dataLatch;
	u16 m_add;
	u8 m_bufferedData;
	u8 m_securityValue;
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

class NesPpu : public QObject {
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


	void init();

	void setChipType(ChipType newType);

	int scanlinesCount() const;

	RenderMethod renderMethod() const;
	void setRenderMethod(RenderMethod method);

	int scanline() const;
	void setScanline(int line);

	void setCharacterLatchEnabled(bool on);
	void setExternalLatchEnabled(bool on);

	void setVBlank(bool on);
	void dma(u8 page);

	void processFrameStart();
	void processFrameEnd();
	void processScanlineStart();
	void processScanlineNext();
	void processScanline();
	void processDummyScanline();
	bool checkSprite0HitHere() const;

	bool save(QDataStream &s);
	bool load(QDataStream &s);

	u8 scrollTileYOffset() const;
	u16 tilePageOffset() const;

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

	ChipType m_type;
	RenderMethod m_renderMethod;

	int m_scanline;
	int m_scanlinesPerFrame;
	QRgb *m_scanlineData;
	QRgb *m_scanline0Data;
	QImage ppuFrame;

	bool m_characterLatchEnabled;
	bool m_externalLatchEnabled;

	bool m_vBlankOut;

	u16 nesVramAddress;
	u16 m_refreshLatch;
	u8 m_scrollTileXOffset;
	u8 m_scrollTileYOffset;
	u16 m_tilePageOffset;
	u16 m_spritePageOffset;
	u16 m_loopyShift;
	u8 m_bgWritten[33];
	u8 m_bit2Rev[256];

	u8 m_spriteMemory[NumSprites*4];

	bool m_spriteClippingEnable;

	friend class NesPpuRegisters;
	friend class NesPpuPalette;
};

inline int NesPpu::scanlinesCount() const
{ return m_scanlinesPerFrame; }
inline NesPpu::RenderMethod NesPpu::renderMethod() const
{ return m_renderMethod; }

inline int NesPpu::scanline() const
{ return m_scanline; }
inline void NesPpu::setVBlank(bool on)
{ m_registers->setVBlank(on); }

inline u8 NesPpu::scrollTileYOffset() const
{ return m_scrollTileYOffset; }
inline u16 NesPpu::tilePageOffset() const
{ return m_tilePageOffset; }

inline bool NesPpu::isSpriteClippingEnabled() const
{ return m_spriteClippingEnable; }

extern QImage ppuFrame;
extern NesPpuPalette nesPpuPalette;
extern NesPpuRegisters nesPpuRegisters;
extern NesPpu nesPpu;

#endif // NESPPU_H
