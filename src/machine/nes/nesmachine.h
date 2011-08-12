#ifndef NESMACHINE_H
#define NESMACHINE_H

class NesCpu;
class NesPpu;
class NesMapper;
class NesCpuMemoryMapper;
class NesPpuMemoryMapper;
class NesDisk;
class NesPad;
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
#define NES_PAL_FRAMERATE	53.355

#define NES_NTSC_CPU_CLK_PER_SCANLINE	1364.0/12.0
#define NES_PAL_CPU_CLK_PER_SCANLINE	1598.0/15.0

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

	NesCpu *cpu() const;
	NesPpu *ppu() const;
	NesPad *pad0() const;
	NesPad *pad1() const;
	NesMapper *mapper() const;
signals:
	void frameGenerated();
private slots:
	void clock();
private:
	Type m_type;
	NesCpu *m_cpu;
	NesPpu *m_ppu;
	NesPad *m_pad0;
	NesPad *m_pad1;
	NesDisk *m_disk;
	NesMapper *m_mapper;

	bool m_running;
	qreal m_cycles;
	QTimer *m_timer;
	QTime m_time;
	qreal m_desiredTime;

	HostAudio *m_hostAudio;
	QAudioFormat *m_hostAudioFormat;
};

inline NesMachine::Type NesMachine::type() const
{ return m_type; }
inline NesCpu *NesMachine::cpu() const
{ return m_cpu; }
inline NesPpu *NesMachine::ppu() const
{ return m_ppu; }
inline NesPad *NesMachine::pad0() const
{ return m_pad0; }
inline NesPad *NesMachine::pad1() const
{ return m_pad1; }
inline NesDisk *NesMachine::disk() const
{ return m_disk; }
inline NesMapper *NesMachine::mapper() const
{ return m_mapper; }

#endif // NESMACHINE_H
