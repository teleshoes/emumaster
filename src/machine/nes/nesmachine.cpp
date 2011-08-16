#include "nesmachine.h"
#include "nescpu.h"
#include "nesppu.h"
#include "nesapu.h"
#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"
#include "nesdisk.h"
#include "nespad.h"
#include <audio/hostaudio.h>
#include <QTimer>

// TODO refactor remove audio remove frameGenerated signal, remove timing

NesMachine::NesMachine(QObject *parent) :
	QObject(parent),
	m_disk(0),
	m_mapper(0),
	m_ppuMapper(0),
	m_running(false) {

	m_cpu = new NesCpu(this);
	m_ppu = new NesPpu(this);
	m_apu = m_cpu->apu();
	m_pad = new NesPad(this);

	QObject::connect(m_ppu, SIGNAL(vblank_o(bool)), m_cpu, SLOT(nmi_i(bool)));

	m_apu->setSampleRate(22050);
	m_apu->setStereoEnabled(true);

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
	QObject::connect(m_timer, SIGNAL(timeout()), SLOT(emulateNextFrame()));
}

NesMachine::~NesMachine() {
	delete m_hostAudioFormat;
}

void NesMachine::reset() {
	m_cpu->nes_reset_i(true);
	// TODO clear reset line
	m_pad->reset();
	m_mapper->reset();
	m_cpuCycleCounter = 0;
	m_ppuCycleCounter = 0;
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
	m_cpu->setMapper(m_mapper->cpuMapper());
	m_ppuMapper = m_mapper->ppuMapper();
	m_ppu->setMapper(m_ppuMapper);
	m_type = disk->machineType();
	// TODO VS system
	if (m_type == NTSC) {
		m_ppu->setChipType(NesPpu::PPU2C02);
		m_scanlineCycles = NES_NTSC_SCANLINE_CLOCKS;
		m_hDrawCycles = 1024;
		m_hBlankCycles = 340;
		m_scanlineEndCycles = 4;
	} else {
		m_ppu->setChipType(NesPpu::PPU2C07);
		m_scanlineCycles = NES_PAL_SCANLINE_CLOCKS;
		m_hDrawCycles = 1200;
		m_hBlankCycles = 398;
		m_scanlineEndCycles = 4;
	}
	m_apu->updateMachineType();
	reset();
	return true;
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

void NesMachine::clockCpu(uint cycles) {
	m_ppuCycleCounter += cycles;
	int realCycles;
	if (m_type == NTSC)
		realCycles = (m_ppuCycleCounter/12) - m_cpuCycleCounter;
	else
		realCycles = (m_ppuCycleCounter/15) - m_cpuCycleCounter;
	if (realCycles > 0)
		m_cpuCycleCounter += m_cpu->clock(realCycles);
}

void NesMachine::emulateFrame(bool drawEnabled) {
	processCheatCodes();
	bZapper = false;
	if (m_ppu->renderMethod() == NesPpu::TileRender)
		emulateFrameTile(drawEnabled);
	else
		emulateFrameNoTile(drawEnabled);
	m_cpu->nes_reset_i(false);

	int size;
	const char *data = m_apu->grabBuffer(&size);
	m_hostAudio->write(data, size);
}

inline void NesMachine::updateZapper(int scanline) {
	if (m_pad->isZapperMode())
		bZapper = (scanline == ZapperY);
}

void NesMachine::emulateFrameNoTile(bool drawEnabled) {
	NesPpu::RenderMethod renderMethod = m_ppu->renderMethod();
	bool all = (renderMethod < NesPpu::PostRender);
	bool pre = (renderMethod & 1);

	int scanline = 0;
	m_ppu->setScanline(scanline);
	clockCpu(all ? m_scanlineCycles : m_hDrawCycles);
	m_ppu->processFrameStart();
	m_ppu->processScanlineNext();
	m_ppuMapper->horizontalSync(scanline);
	if (all) {
		m_ppu->processScanlineStart();
	} else {
		clockCpu(NesPpu::FetchCycles*32);
		m_ppu->processScanlineStart();
		clockCpu(NesPpu::FetchCycles*10 + m_scanlineEndCycles);
	}
	updateZapper(scanline);

	for (scanline = 1; scanline < 240; scanline++) {
		m_ppu->setScanline(scanline);
		if (!pre)
			clockCpu(all ? m_scanlineCycles : m_hDrawCycles);
		if (drawEnabled) {
			m_ppu->processScanline();
		} else {
			if (m_pad->isZapperMode() && scanline == ZapperY ) {
				m_ppu->processScanline();
			} else {
				if (m_ppu->checkSprite0HitHere())
					m_ppu->processScanline();
				else
					m_ppu->processDummyScanline();
			}
		}
		if (all) {
			m_ppu->processScanlineNext();
			if (pre)
				clockCpu(m_scanlineCycles);
			m_ppuMapper->horizontalSync(scanline);
			m_ppu->processScanlineStart();
		} else {
			if (pre)
				clockCpu(m_hDrawCycles);
			m_ppu->processScanlineNext();
			m_ppuMapper->horizontalSync(scanline);
			clockCpu(NesPpu::FetchCycles*32);
			m_ppu->processScanlineStart();
			clockCpu(NesPpu::FetchCycles*10 + m_scanlineEndCycles);
		}
		updateZapper(scanline);
	}

	scanline = 240;
	m_ppu->setScanline(scanline);
	m_ppuMapper->verticalSync();
	if (all) {
		clockCpu(m_scanlineCycles);
		m_ppuMapper->horizontalSync(scanline);
	} else {
		clockCpu(m_hDrawCycles);
		m_ppuMapper->horizontalSync(scanline);
		clockCpu(m_hBlankCycles);
	}
	updateZapper(scanline);

	int totalScanlines = m_ppu->scanlinesCount();
	for (scanline = 241; scanline <= totalScanlines-1; scanline++) {
		m_ppu->setScanline(scanline);

		if (scanline == 241)
			m_ppu->setVBlank(true);
		else if (scanline == totalScanlines-1)
			m_ppu->setVBlank(false);

		if (all) {
			clockCpu(m_scanlineCycles);
			m_ppuMapper->horizontalSync(scanline);
		} else {
			clockCpu(m_hDrawCycles);
			m_ppuMapper->horizontalSync(scanline);
			clockCpu(m_hBlankCycles);
		}

		if (scanline != totalScanlines-1)
			updateZapper(scanline);
	}
}

inline void NesMachine::emulateVisibleScanlineTile(int scanline) {
	m_ppu->processScanlineNext();
	clockCpu(NesPpu::FetchCycles*10);
	m_ppuMapper->horizontalSync(scanline);
	clockCpu(NesPpu::FetchCycles*22);
	m_ppu->processScanlineStart();
	clockCpu(NesPpu::FetchCycles*10 + m_scanlineEndCycles);
	updateZapper(scanline);
}

void NesMachine::emulateFrameTile(bool drawEnabled) {
	int scanline = 0;
	m_ppu->setScanline(scanline);
	clockCpu(NesPpu::FetchCycles*128);
	m_ppu->processFrameStart();
	emulateVisibleScanlineTile(scanline);

	if (drawEnabled) {
		for (scanline = 1; scanline < 240; scanline++) {
			m_ppu->setScanline(scanline);
			m_ppu->processScanline();
			emulateVisibleScanlineTile(scanline);
		}
	} else {
		for (scanline = 1; scanline < 240; scanline++) {
			m_ppu->setScanline(scanline);
			if (m_pad->isZapperMode() && scanline == ZapperY)
				m_ppu->processScanline();
			else {
				if (m_ppu->checkSprite0HitHere()) {
					m_ppu->processScanline();
				} else {
					clockCpu(NesPpu::FetchCycles*128);
					m_ppu->processDummyScanline();
				}
			}
			emulateVisibleScanlineTile(scanline);
		}
	}

	scanline = 240;
	m_ppu->setScanline(scanline);
	m_ppuMapper->verticalSync();
	clockCpu(m_hDrawCycles);
	m_ppuMapper->horizontalSync(scanline);
	clockCpu(m_hBlankCycles);
	updateZapper(scanline);

	int totalScanlines = m_ppu->scanlinesCount();
	for (scanline = 241; scanline <= totalScanlines-1; scanline++) {
		m_ppu->setScanline(scanline);

		if (scanline == 241)
			m_ppu->setVBlank(true);
		else if (scanline == totalScanlines-1)
			m_ppu->setVBlank(false);

		clockCpu(m_hDrawCycles);
		m_ppuMapper->horizontalSync(scanline);
		clockCpu(m_hBlankCycles);

		if (scanline != totalScanlines-1)
			updateZapper(scanline);
	}
}

void NesMachine::processCheatCodes() {
	// TODO cheat codes
}

void NesMachine::emulateNextFrame() {
	emulateFrame(true);

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
