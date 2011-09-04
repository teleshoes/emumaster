#include "nesmachine.h"
#include "nescpu.h"
#include "nesppu.h"
#include "nesapu.h"
#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"
#include "nesdisk.h"
#include "nespad.h"
#include "nesmachine.h"
#include "gamegeniecode.h"
#include "gamegeniecodelistmodel.h"
#include "machineview.h"
#include <QSettings>
#include <QApplication>
#include <QtDeclarative>

// TODO machineView
/*
m_machine->setGameGenieCodeList(m_gameGenieCodeListModel->enabledList());
*/

NesMachine::NesMachine(QObject *parent) :
	IMachine("nes", parent),
	m_disk(0),
	m_mapper(0),
	m_ppuMapper(0) {

	setVideoSrcRect(QRectF(8.0f, 1.0f, NesPpu::VisibleScreenWidth, NesPpu::VisibleScreenHeight));

	m_cpu = new NesCpu(this);
	m_ppu = new NesPpu(this);
	m_apu = m_cpu->apu();
	m_pad = new NesPad(this);

	QObject::connect(m_ppu, SIGNAL(vblank_o(bool)), m_cpu, SLOT(nmi_i(bool)));

	// TODOm_gameGenieCodeListModel = new GameGenieCodeListModel(this);
	// m_gameGenieCodeListModel->load();

	qmlRegisterType<NesPpu>();
	qmlRegisterType<NesCpu>();
	qmlRegisterType<NesDisk>();
	qmlRegisterType<NesMapper>();
}

NesMachine::~NesMachine() {
}

void NesMachine::reset() {
	m_mapper->reset();
	m_cpu->nes_reset_i(true);
	// TODO maybe 2 reset frames
	m_pad->reset();
	m_cpuCycleCounter = 0;
	m_ppuCycleCounter = 0;
}

QString NesMachine::setDisk(const QString &path) {
	NesDisk *disk = new NesDisk(QString("%1.nes").arg(path));
	if (!disk->isLoaded())
		return "Could not load ROM file";

	delete m_disk;
	delete m_mapper;

	m_disk = disk;
	disk->setParent(this);
	m_mapper = NesMapper::load(this, disk->mapperType());
	if (!m_mapper)
		return QString("Mapper %1 is not supported").arg(disk->mapperType());
	m_cpu->setMapper(m_mapper->cpuMapper());
	m_ppuMapper = m_mapper->ppuMapper();
	m_ppu->setMapper(m_ppuMapper);
	m_type = disk->machineType();
	// TODO VS system
	if (m_type == NTSC) {
		m_ppu->setChipType(NesPpu::PPU2C02);
		setFrameRate(NES_NTSC_FRAMERATE);
		m_scanlineCycles = NES_NTSC_SCANLINE_CLOCKS;
		m_hDrawCycles = 1024;
		m_hBlankCycles = 340;
		m_scanlineEndCycles = 4;
	} else {
		m_ppu->setChipType(NesPpu::PPU2C07);
		setFrameRate(NES_PAL_FRAMERATE);
		m_scanlineCycles = NES_PAL_SCANLINE_CLOCKS;
		m_hDrawCycles = 1200;
		m_hBlankCycles = 398;
		m_scanlineEndCycles = 4;
	}
	m_apu->updateMachineType();
	reset();
	return QString();
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

void NesMachine::setPadKey(IMachine::PadKey key, bool state) {
	switch (key) {
	case Left_PadKey:		m_pad->setButtonState(0, NesPad::Left, state); break;
	case Right_PadKey:		m_pad->setButtonState(0, NesPad::Right, state); break;
	case Up_PadKey:			m_pad->setButtonState(0, NesPad::Up, state); break;
	case Down_PadKey:		m_pad->setButtonState(0, NesPad::Down, state); break;
	case A_PadKey:			m_pad->setButtonState(0, NesPad::A, state); break;
	case B_PadKey:			m_pad->setButtonState(0, NesPad::B, state); break;
	case Start_PadKey:		m_pad->setButtonState(0, NesPad::Start, state); break;
	case Select_PadKey:		m_pad->setButtonState(0, NesPad::Select, state); break;
	case AllKeys:			m_pad->clearButtons(0); break;
	default: break;
	}
}

const QImage &NesMachine::frame() const
{ return m_ppu->frame(); }

void NesMachine::emulateFrame(bool drawEnabled) {
	processCheatCodes();
	bZapper = false;
	if (m_ppu->renderMethod() == NesPpu::TileRender)
		emulateFrameTile(drawEnabled);
	else
		emulateFrameNoTile(drawEnabled);
	m_cpu->nes_reset_i(false);
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
	m_ppu->processFrameEnd();
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
	m_ppu->processFrameEnd();
}

void NesMachine::processCheatCodes() {
	// TODO cheat codes
}

bool NesMachine::save(QDataStream &s) {
	if (!m_cpu->save(s))
		return false;
	if (!m_ppu->save(s))
		return false;
	s << m_cpuCycleCounter;
	s << m_ppuCycleCounter;
	return true;
}

bool NesMachine::load(QDataStream &s) {
	if (!m_cpu->load(s))
		return false;
	if (!m_ppu->load(s))
		return false;
	s >> m_cpuCycleCounter;
	s >> m_ppuCycleCounter;
	return true;
}

void NesMachine::saveSettings(QSettings &s) {
	IMachine::saveSettings(s);
	// TODO save rendering type
}

void NesMachine::loadSettings(QSettings &s) {
	IMachine::loadSettings(s);
}

int NesMachine::fillAudioBuffer(char *stream, int streamSize)
{ return m_apu->fillBuffer(stream, streamSize); }
void NesMachine::setAudioSampleRate(int sampleRate)
{ m_apu->setSampleRate(sampleRate); }

bool NesMachine::isGameGenieCodeValid(const QString &s) {
	GameGenieCode code;
	return code.parse(s);
}

int main(int argc, char *argv[]) {
	if (argc < 2)
		return -1;
	QApplication app(argc, argv);
	MachineView view(new NesMachine(), argv[1]);
	return app.exec();
}
