#include "machine.h"
#include "cpu.h"
#include "ppu.h"
#include "apu.h"
#include "disk.h"
#include "pad.h"
#include "machine.h"
#include "gamegeniecode.h"
#include "gamegeniecodelistmodel.h"
#include "machineview.h"
#include <QSettings>
#include <QApplication>
#include <QtDeclarative>

static u32 scanlineCycles;
static u32 scanlineEndCycles;
static u32 hDrawCycles;
static u32 hBlankCycles;

static u64 cpuCycleCounter;
static u64 ppuCycleCounter;

static bool bZapper; // TODO zapper
static int ZapperY;

NesMachine nesMachine;

NesMachine::NesMachine() :
	IMachine("nes") {
}

QString NesMachine::init() {
	// TODO QObject::connect(nesPpu, SIGNAL(vblank_o(bool)), nesCpu, SLOT(nmi_i(bool)));
	qmlRegisterType<GameGenieCodeListModel>();
	setVideoSrcRect(QRectF(8.0f, 1.0f, NesPpu::VisibleScreenWidth, NesPpu::VisibleScreenHeight));
	nesCpu.init();
	nesPad.init();
	return QString();
}

void NesMachine::shutdown() {
	// TODO ppuFrame = QImage()
}

void NesMachine::reset() {
	nesMapper->reset();
	nesCpu.reset_i(true);
	cpuCycleCounter = 0;
	ppuCycleCounter = 0;
}

QString NesMachine::setDisk(const QString &path) {
	if (!nesDisk.load(path))
		return "Could not load ROM file";

	m_mapper = NesMapper::create(this, disk->mapperType());
	if (!m_mapper)
		return QString("Mapper %1 is not supported").arg(disk->mapperType());

	QString diskInfo = QString("Disk Info: Mapper %1(%2), CRC: %3, %4 System")
			.arg(nesMapper->name())
			.arg(mapperType)
			.arg(nesDiskCrc, 8, 16)
			.arg((nesSystemType == NES_PAL) ? "PAL" : "NTSC");
	printf(qPrintable(diskInfo));

	// TODO VS system
	if (nesSystemType == NES_NTSC) {
		nesPpu->setChipType(NesPpu::PPU2C02);
		setFrameRate(NES_NTSC_FRAMERATE);
		scanlineCycles = NES_NTSC_SCANLINE_CLOCKS;
		hDrawCycles = 1024;
		hBlankCycles = 340;
		scanlineEndCycles = 4;
	} else {
		nesPpu->setChipType(NesPpu::PPU2C07);
		setFrameRate(NES_PAL_FRAMERATE);
		scanlineCycles = NES_PAL_SCANLINE_CLOCKS;
		hDrawCycles = 1200;
		hBlankCycles = 398;
		scanlineEndCycles = 4;
	}
	m_apu->updateMachineType();
	reset();
	return QString();
}

void NesMachine::clockCpu(uint cycles) {
	ppuCycleCounter += cycles;
	int realCycles;
	if (m_type == NES_NTSC)
		realCycles = (ppuCycleCounter/12) - cpuCycleCounter;
	else
		realCycles = (ppuCycleCounter/15) - cpuCycleCounter;
	if (realCycles > 0)
		cpuCycleCounter += nesCpu.clock(realCycles);
}

const QImage &NesMachine::frame() const
{ return ppuFrame; }

void NesMachine::emulateFrame(bool drawEnabled) {
	bZapper = false;
	if (nesPpu->renderMethod() == NesPpu::TileRender)
		emulateFrameTile(drawEnabled);
	else
		emulateFrameNoTile(drawEnabled);
	nesCpu->nes_reset_i(false);
}

inline void NesMachine::updateZapper(int scanline) {
	if (m_pad->isZapperMode())
		bZapper = (scanline == ZapperY);
}

void NesMachine::emulateFrameNoTile(bool drawEnabled) {
	NesPpu::RenderMethod renderMethod = nesPpu->renderMethod();
	bool all = (renderMethod < NesPpu::PostRender);
	bool pre = (renderMethod & 1);

	int scanline = 0;
	nesPpu->setScanline(scanline);
	clockCpu(all ? scanlineCycles : hDrawCycles);
	nesPpu->processFrameStart();
	nesPpu->processScanlineNext();
	m_mapper->horizontalSync(scanline);
	if (all) {
		nesPpu->processScanlineStart();
	} else {
		clockCpu(NesPpu::FetchCycles*32);
		nesPpu->processScanlineStart();
		clockCpu(NesPpu::FetchCycles*10 + scanlineEndCycles);
	}
	updateZapper(scanline);

	for (scanline = 1; scanline < 240; scanline++) {
		nesPpu->setScanline(scanline);
		if (!pre)
			clockCpu(all ? scanlineCycles : hDrawCycles);
		if (drawEnabled) {
			nesPpu->processScanline();
		} else {
			if (m_pad->isZapperMode() && scanline == ZapperY ) {
				nesPpu->processScanline();
			} else {
				if (nesPpu->checkSprite0HitHere())
					nesPpu->processScanline();
				else
					nesPpu->processDummyScanline();
			}
		}
		if (all) {
			nesPpu->processScanlineNext();
			if (pre)
				clockCpu(scanlineCycles);
			m_mapper->horizontalSync(scanline);
			nesPpu->processScanlineStart();
		} else {
			if (pre)
				clockCpu(hDrawCycles);
			nesPpu->processScanlineNext();
			m_mapper->horizontalSync(scanline);
			clockCpu(NesPpu::FetchCycles*32);
			nesPpu->processScanlineStart();
			clockCpu(NesPpu::FetchCycles*10 + scanlineEndCycles);
		}
		updateZapper(scanline);
	}

	scanline = 240;
	nesPpu->setScanline(scanline);
	m_mapper->verticalSync();
	if (all) {
		clockCpu(scanlineCycles);
		m_mapper->horizontalSync(scanline);
	} else {
		clockCpu(hDrawCycles);
		m_mapper->horizontalSync(scanline);
		clockCpu(hBlankCycles);
	}
	updateZapper(scanline);

	int totalScanlines = nesPpu->scanlinesCount();
	for (scanline = 241; scanline <= totalScanlines-1; scanline++) {
		nesPpu->setScanline(scanline);

		if (scanline == 241)
			nesPpu->setVBlank(true);
		else if (scanline == totalScanlines-1)
			nesPpu->setVBlank(false);

		if (all) {
			clockCpu(scanlineCycles);
			m_mapper->horizontalSync(scanline);
		} else {
			clockCpu(hDrawCycles);
			m_mapper->horizontalSync(scanline);
			clockCpu(hBlankCycles);
		}

		if (scanline != totalScanlines-1)
			updateZapper(scanline);
	}
	nesPpu->processFrameEnd();
}

inline void NesMachine::emulateVisibleScanlineTile(int scanline) {
	nesPpu->processScanlineNext();
	clockCpu(NesPpu::FetchCycles*10);
	m_mapper->horizontalSync(scanline);
	clockCpu(NesPpu::FetchCycles*22);
	nesPpu->processScanlineStart();
	clockCpu(NesPpu::FetchCycles*10 + scanlineEndCycles);
	updateZapper(scanline);
}

void NesMachine::emulateFrameTile(bool drawEnabled) {
	int scanline = 0;
	nesPpu->setScanline(scanline);
	clockCpu(NesPpu::FetchCycles*128);
	nesPpu->processFrameStart();
	emulateVisibleScanlineTile(scanline);

	if (drawEnabled) {
		for (scanline = 1; scanline < 240; scanline++) {
			nesPpu->setScanline(scanline);
			nesPpu->processScanline();
			emulateVisibleScanlineTile(scanline);
		}
	} else {
		for (scanline = 1; scanline < 240; scanline++) {
			nesPpu->setScanline(scanline);
			if (m_pad->isZapperMode() && scanline == ZapperY)
				nesPpu->processScanline();
			else {
				if (nesPpu->checkSprite0HitHere()) {
					nesPpu->processScanline();
				} else {
					clockCpu(NesPpu::FetchCycles*128);
					nesPpu->processDummyScanline();
				}
			}
			emulateVisibleScanlineTile(scanline);
		}
	}

	scanline = 240;
	nesPpu->setScanline(scanline);
	m_mapper->verticalSync();
	clockCpu(hDrawCycles);
	m_mapper->horizontalSync(scanline);
	clockCpu(hBlankCycles);
	updateZapper(scanline);

	int totalScanlines = nesPpu->scanlinesCount();
	for (scanline = 241; scanline <= totalScanlines-1; scanline++) {
		nesPpu->setScanline(scanline);

		if (scanline == 241)
			nesPpu->setVBlank(true);
		else if (scanline == totalScanlines-1)
			nesPpu->setVBlank(false);

		clockCpu(hDrawCycles);
		m_mapper->horizontalSync(scanline);
		clockCpu(hBlankCycles);

		if (scanline != totalScanlines-1)
			updateZapper(scanline);
	}
	nesPpu->processFrameEnd();
}

int NesMachine::fillAudioBuffer(char *stream, int streamSize)
{ return m_apu->fillBuffer(stream, streamSize); }
void NesMachine::setAudioSampleRate(int sampleRate)
{ m_apu->setSampleRate(sampleRate); }

#define STATE_SERIALIZE_BUILDER(sl) \
STATE_SERIALIZE_BEGIN_##sl(NesMachine, 1) \
	STATE_SERIALIZE_SUBCALL_PTR_##sl(m_mapper) \
	STATE_SERIALIZE_SUBCALL_PTR_##sl(m_apu) \
	STATE_SERIALIZE_SUBCALL_##sl(nesCpu) \
	STATE_SERIALIZE_SUBCALL_##sl(nesPpu) \
	STATE_SERIALIZE_VAR_##sl(cpuCycleCounter) \
	STATE_SERIALIZE_VAR_##sl(ppuCycleCounter) \
STATE_SERIALIZE_END_##sl(NesMachine)

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
