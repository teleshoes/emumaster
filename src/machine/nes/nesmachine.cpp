#include "nesmachine.h"
#include "nescpu.h"
#include "nesppu.h"
#include "nesapu.h"
#include "nesmapper.h"
#include "nescpumemorymapper.h"
#include "nesppumemorymapper.h"
#include "nesdisk.h"
#include "nespad.h"
#include <audio/hostaudio.h>
#include <QTimer>

// TODO zmienić tam gdzie powinno być quint16

NesMachine::NesMachine(QObject *parent) :
	QObject(parent),
	m_disk(0),
	m_mapper(0),
	m_running(false) {

	m_cpu = new NesCpu(this);
	m_ppu = new NesPpu(this);
	m_pad0 = new NesPad(this);
	m_pad1 = new NesPad(this);

	QObject::connect(m_ppu, SIGNAL(vblank_o(bool)), m_cpu, SLOT(nmi_i(bool)));

	NesApu *apu = m_cpu->apu();
	apu->setSampleRate(22050);
	apu->setStereoEnabled(true);

	m_hostAudioFormat = new QAudioFormat();
	m_hostAudioFormat->setSampleRate(22050);
	m_hostAudioFormat->setChannelCount(2);
	m_hostAudioFormat->setSampleSize(16);
	m_hostAudioFormat->setCodec("audio/pcm");
	m_hostAudioFormat->setByteOrder(QAudioFormat::LittleEndian);
	m_hostAudioFormat->setSampleType(QAudioFormat::UnSignedInt);
	m_hostAudio = new HostAudio(this);
	m_hostAudio->setFormat(*m_hostAudioFormat);

	m_timer = new QTimer(this);
	m_timer->setSingleShot(true);
	QObject::connect(m_timer, SIGNAL(timeout()), SLOT(clock()));
}

NesMachine::~NesMachine() {
	delete m_hostAudioFormat;
}

void NesMachine::reset() {
	if (m_running) {
		m_cpu->reset_i(true);
		m_cycles += 600;
		m_cpu->clockTo(m_cycles);
		m_cpu->reset_i(false);
	}
}

bool NesMachine::setDisk(NesDisk *disk) {
	m_running = false;
	if (!disk->isLoaded())
		return false;
	delete m_disk;
	delete m_mapper;
	m_disk = disk;
	disk->setParent(this);
	m_mapper = NesMapper::load(this, disk->mapperType());
	if (!m_mapper)
		return false;
	m_type = disk->machineType();
	m_cycles = 0;
	m_cpu->setMemory(m_mapper->cpuMemory());
	m_ppu->setMemory(m_mapper->ppuMemory());
	m_ppu->setChipType((m_type == NTSC) ? NesPpu::PPU2C02 : NesPpu::PPU2C07);
	m_cpu->apu()->updateFrameRate();
	return true;
}

void NesMachine::setRunning(bool run) {
	if (m_running == run)
		return;
	if (run) {
		if (m_disk) {
			run = m_running;
			m_timer->start(0);
			m_running = true;
			m_time.start();
			m_desiredTime = 0.0;
		}
	} else {
		m_running = false;
		m_timer->stop();
	}
}

void NesMachine::clock() {
	bool lastLine = false;
	qreal cyclesPerScanline = ((m_type == NTSC) ? NES_NTSC_CPU_CLK_PER_SCANLINE : NES_PAL_CPU_CLK_PER_SCANLINE);
	while (!lastLine) {
		m_cycles += cyclesPerScanline;
		m_cpu->clockTo(m_cycles);
		m_ppu->processScanline(&lastLine);
	}
	int size;
	const char *data = m_cpu->apu()->grabBuffer(&size);
	m_hostAudio->write(data, size);

	m_desiredTime += 1000.0 / ((m_type == NTSC) ? NES_NTSC_FRAMERATE : NES_PAL_FRAMERATE);
	if (m_time.elapsed() > m_desiredTime) {
		m_desiredTime = 0.0;
		m_time.start();
		m_timer->start();
	} else {
		m_timer->start(m_desiredTime - m_time.elapsed());
	}
	emit frameGenerated();
}

void NesMachine::setHostAudioSampleRate(int rate) {
	if (rate == m_hostAudioFormat->sampleRate())
		return;
	m_cpu->apu()->setSampleRate(rate);
	m_hostAudioFormat->setSampleRate(rate);
	m_hostAudio->setFormat(*m_hostAudioFormat);
}

void NesMachine::setHostAudioStereoEnabled(bool on) {
	int nChannels = (on ? 2 : 1);
	if (nChannels == m_hostAudioFormat->channelCount())
		return;
	m_cpu->apu()->setStereoEnabled(on);
	m_hostAudioFormat->setChannelCount(nChannels);
	m_hostAudio->setFormat(*m_hostAudioFormat);
}

QAudioFormat NesMachine::hostAudioFormat() const
{ return *m_hostAudioFormat; }
