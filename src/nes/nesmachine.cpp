#include "nesmachine.h"
#include "nescpu.h"
#include "nesppu.h"
#include "nesapu.h"

#include "nesdisk.h"
#include "nespad.h"
#include "nesmachine.h"
#include "gamegeniecode.h"
#include "gamegeniecodelistmodel.h"
#include "machineview.h"
#include <QSettings>
#include <QApplication>
#include <QtDeclarative>

NesMachine::NesMachine(QObject *parent) :
	IMachine("nes", parent),
	m_disk(0),
	m_mapper(0) {

	setVideoSrcRect(QRectF(8.0f, 1.0f, NesPpu::VisibleScreenWidth, NesPpu::VisibleScreenHeight));

	m_cpu = new NesCpu(this);
	m_ppu = new NesPpu(this);
	m_apu = m_cpu->apu();
	m_pad = new NesPad(this);

	QObject::connect(m_ppu, SIGNAL(vblank_o(bool)), m_cpu, SLOT(nmi_i(bool)));

	qmlRegisterType<NesPpu>();
	qmlRegisterType<NesCpu>();
	qmlRegisterType<NesDisk>();
	qmlRegisterType<NesMapper>();
	qmlRegisterType<GameGenieCodeListModel>();
}

NesMachine::~NesMachine() {
}

void NesMachine::reset() {
	m_mapper->reset();
	m_cpu->nes_reset_i(true);
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
	m_mapper = NesMapper::create(this, disk->mapperType());
	if (!m_mapper)
		return QString("Mapper %1 is not supported").arg(disk->mapperType());
	m_cpu->setMapper(m_mapper);
	m_ppu->setMapper(m_mapper);
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

void NesMachine::setPadKey(PadKey key, bool state) {
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
	m_mapper->horizontalSync(scanline);
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
			m_mapper->horizontalSync(scanline);
			m_ppu->processScanlineStart();
		} else {
			if (pre)
				clockCpu(m_hDrawCycles);
			m_ppu->processScanlineNext();
			m_mapper->horizontalSync(scanline);
			clockCpu(NesPpu::FetchCycles*32);
			m_ppu->processScanlineStart();
			clockCpu(NesPpu::FetchCycles*10 + m_scanlineEndCycles);
		}
		updateZapper(scanline);
	}

	scanline = 240;
	m_ppu->setScanline(scanline);
	m_mapper->verticalSync();
	if (all) {
		clockCpu(m_scanlineCycles);
		m_mapper->horizontalSync(scanline);
	} else {
		clockCpu(m_hDrawCycles);
		m_mapper->horizontalSync(scanline);
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
			m_mapper->horizontalSync(scanline);
		} else {
			clockCpu(m_hDrawCycles);
			m_mapper->horizontalSync(scanline);
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
	m_mapper->horizontalSync(scanline);
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
	m_mapper->verticalSync();
	clockCpu(m_hDrawCycles);
	m_mapper->horizontalSync(scanline);
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
		m_mapper->horizontalSync(scanline);
		clockCpu(m_hBlankCycles);

		if (scanline != totalScanlines-1)
			updateZapper(scanline);
	}
	m_ppu->processFrameEnd();
}

void NesMachine::processCheatCodes() {
	// TODO cheat codes
}

int NesMachine::fillAudioBuffer(char *stream, int streamSize)
{ return m_apu->fillBuffer(stream, streamSize); }
void NesMachine::setAudioSampleRate(int sampleRate)
{ m_apu->setSampleRate(sampleRate); }

#define STATE_SERIALIZE_BUILDER(sl) \
	STATE_SERIALIZE_BEGIN_##sl(NesMachine) \
	STATE_SERIALIZE_SUBCALL_PTR_##sl(m_cpu) \
	STATE_SERIALIZE_SUBCALL_PTR_##sl(m_ppu) \
	STATE_SERIALIZE_VAR_##sl(m_cpuCycleCounter) \
	STATE_SERIALIZE_VAR_##sl(m_ppuCycleCounter) \
	STATE_SERIALIZE_END(NesMachine)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)

int main(int argc, char *argv[]) {
	if (argc < 2)
		return -1;
	QApplication app(argc, argv);
	MachineView view(new NesMachine(), argv[1]);
	GameGenieCodeListModel gameGenie(static_cast<NesMachine *>(view.machine()));
	view.settingsView()->rootContext()->setContextProperty("gameGenie", static_cast<QObject *>(&gameGenie));
	return app.exec();
}
