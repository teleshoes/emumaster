#ifndef NESMACHINE_H
#define NESMACHINE_H

#include <imachine.h>

#define NES_PPU_NTSC_CLK	21477270.0
#define NES_PPU_PAL_CLK		26601712.0

#define NES_CPU_NTSC_CLK	NES_PPU_NTSC_CLK/12.0
#define NES_CPU_PAL_CLK		NES_PPU_PAL_CLK/15.0

#define NES_NTSC_FRAMERATE	60.098
#define NES_PAL_FRAMERATE	50.007

#define NES_NTSC_SCANLINE_CLOCKS	1364
#define NES_PAL_SCANLINE_CLOCKS		1598

enum SystemType { NES_NTSC, NES_PAL };

class NesMachine : public IMachine {
	Q_OBJECT
public:
	NesMachine();
	QString init();
	void shutdown();
	void reset();

	QString setDisk(const QString &path);

	void clockCpu(u32 cycles);
	const QImage &frame() const;
	void emulateFrame(bool drawEnabled);
	int fillAudioBuffer(char *stream, int streamSize);
	void setPadKeys(int pad, int keys);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	void emulateFrameNoTile(bool drawEnabled);
	void emulateVisibleScanlineNoTile();

	void emulateFrameTile(bool drawEnabled);
	void emulateVisibleScanlineTile();

	void updateZapper();
};

extern NesMachine nesMachine;
extern SystemType nesSystemType;

#endif // NESMACHINE_H
