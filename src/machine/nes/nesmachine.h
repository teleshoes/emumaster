#ifndef NESMACHINE_H
#define NESMACHINE_H

class NesCpu;
class NesPpu;
class NesApu;
class NesPad;
class NesDisk;
class NesMapper;
class NesCpuMapper;
class NesPpuMapper;
class HostAudio;
#include "nes_global.h"
#include <QObject>
#include <QTime>
class QTimer;
class QAudioFormat;

#define NES_PPU_NTSC_CLK	21477270.0
#define NES_PPU_PAL_CLK		26601712.0

#define NES_CPU_NTSC_CLK	NES_PPU_NTSC_CLK/12.0
#define NES_CPU_PAL_CLK		NES_PPU_PAL_CLK/15.0

#define NES_NTSC_FRAMERATE	60.098
#define NES_PAL_FRAMERATE	50.007

#define NES_NTSC_SCANLINE_CLOCKS	1364
#define NES_PAL_SCANLINE_CLOCKS		1598

class NES_EXPORT NesMachine : public QObject {
	Q_OBJECT
public:
	enum Type { NTSC, PAL };

	explicit NesMachine(QObject *parent = 0);
	~NesMachine();
	void reset();

	Type type() const;

	NesDisk *disk() const;
	bool setDisk(NesDisk *disk);

	void setHostAudioSampleRate(int rate);
	void setHostAudioStereoEnabled(bool on);
	QAudioFormat hostAudioFormat() const;

	void setRunning(bool on);
	void clockCpu(uint cycles);

	NesCpu *cpu() const;
	NesPpu *ppu() const;
	NesApu *apu() const;
	NesPad *pad() const;
	NesMapper *mapper() const;
private slots:
	void emulateNextFrame();
	void emulateFrame(bool drawEnabled);
signals:
	void frameGenerated();
private:
	void emulateFrameNoTile(bool drawEnabled);
	void emulateVisibleScanlineNoTile(int scanline);

	void emulateFrameTile(bool drawEnabled);
	void emulateVisibleScanlineTile(int scanline);

	void updateZapper(int scanline);
	void processCheatCodes();

	Type m_type;
	NesCpu *m_cpu;
	NesPpu *m_ppu;
	NesApu *m_apu;
	NesPad *m_pad;
	NesDisk *m_disk;
	NesMapper *m_mapper;
	NesPpuMapper *m_ppuMapper;

	bool m_running;
	QTimer *m_timer;
	QTime m_time;
	qreal m_desiredTime;

	uint m_scanlineCycles;
	uint m_scanlineEndCycles;
	uint m_hDrawCycles;
	uint m_hBlankCycles;

	quint64 m_cpuCycleCounter;
	quint64 m_ppuCycleCounter;

	HostAudio *m_hostAudio;
	QAudioFormat *m_hostAudioFormat;

	bool bZapper; // TODO zapper
	int ZapperY;
};

inline NesMachine::Type NesMachine::type() const
{ return m_type; }
inline NesCpu *NesMachine::cpu() const
{ return m_cpu; }
inline NesPpu *NesMachine::ppu() const
{ return m_ppu; }
inline NesApu *NesMachine::apu() const
{ return m_apu; }
inline NesPad *NesMachine::pad() const
{ return m_pad; }
inline NesDisk *NesMachine::disk() const
{ return m_disk; }
inline NesMapper *NesMachine::mapper() const
{ return m_mapper; }

#endif // NESMACHINE_H
