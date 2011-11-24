/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "machine.h"
#include "cpu.h"
#include "ppu.h"
#include "apu.h"
#include "disk.h"
#include "pad.h"
#include "mapper.h"
#include "gamegeniecode.h"
#include "gamegeniecodelistmodel.h"
#include "machineview.h"
#include <QSettings>
#include <QApplication>
#include <QtDeclarative>

NesMachine nesMachine;
SystemType nesSystemType;

static u32 scanlineCycles;
static u32 scanlineEndCycles;
static u32 hDrawCycles;
static u32 hBlankCycles;

static u64 cpuCycleCounter;
static u64 ppuCycleCounter;

static bool bZapper; // TODO zapper
static int ZapperY;

NesMachine::NesMachine() :
	IMachine("nes") {
}

QString NesMachine::init() {
	qmlRegisterType<GameGenieCodeListModel>();
	setVideoSrcRect(QRectF(8.0f, 1.0f, NesPpu::VisibleScreenWidth, NesPpu::VisibleScreenHeight));
	nesCpu.init();
	nesApu.init();
	nesPad.init();
	nesPpu.init();
	qmlRegisterType<NesPpu>();
	return QString();
}

void NesMachine::shutdown() {
	delete nesMapper;
	delete nesVrom;
	delete nesRom;
	nesPpuFrame = QImage();
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

	nesMapper = NesMapper::create(nesMapperType);
	if (!nesMapper)
		return QString("Mapper %1 is not supported").arg(nesMapperType);

	QString diskInfo = QString("Disk Info: Mapper %1(%2), CRC: %3, %4 System\n")
			.arg(nesMapper->name())
			.arg(nesMapperType)
			.arg(nesDiskCrc, 8, 16)
			.arg((nesSystemType == NES_PAL) ? "PAL" : "NTSC");
	printf(qPrintable(diskInfo));

	// TODO VS system
	if (nesSystemType == NES_NTSC) {
		nesPpu.setChipType(NesPpu::PPU2C02);
		setFrameRate(NES_NTSC_FRAMERATE);
		scanlineCycles = NES_NTSC_SCANLINE_CLOCKS;
		hDrawCycles = 1024;
		hBlankCycles = 340;
		scanlineEndCycles = 4;
	} else {
		nesPpu.setChipType(NesPpu::PPU2C07);
		setFrameRate(NES_PAL_FRAMERATE);
		scanlineCycles = NES_PAL_SCANLINE_CLOCKS;
		hDrawCycles = 1200;
		hBlankCycles = 398;
		scanlineEndCycles = 4;
	}
	nesApu.updateMachineType();
	reset();
	return QString();
}

void NesMachine::clockCpu(uint cycles) {
	ppuCycleCounter += cycles;
	int realCycles;
	if (nesSystemType == NES_NTSC)
		realCycles = (ppuCycleCounter/12) - cpuCycleCounter;
	else
		realCycles = (ppuCycleCounter/15) - cpuCycleCounter;
	if (realCycles > 0)
		cpuCycleCounter += nesCpu.clock(realCycles);
}

const QImage &NesMachine::frame() const
{ return nesPpuFrame; }

void NesMachine::emulateFrame(bool drawEnabled) {
	bZapper = false;
	if (nesPpu.renderMethod() == NesPpu::TileRender)
		emulateFrameTile(drawEnabled);
	else
		emulateFrameNoTile(drawEnabled);
	nesCpu.reset_i(false);
}

inline void NesMachine::updateZapper() {
	if (nesPad.isZapperMode())
		bZapper = (nesPpuScanline == ZapperY);
}

void NesMachine::emulateFrameNoTile(bool drawEnabled) {
	NesPpu::RenderMethod renderMethod = nesPpu.renderMethod();
	bool all = (renderMethod < NesPpu::PostRender);
	bool pre = (renderMethod & 1);

	clockCpu(all ? scanlineCycles : hDrawCycles);
	nesPpu.processFrameStart();
	nesPpu.processScanlineNext();
	nesMapper->horizontalSync();
	if (all) {
		nesPpu.processScanlineStart();
	} else {
		clockCpu(NesPpu::FetchCycles*32);
		nesPpu.processScanlineStart();
		clockCpu(NesPpu::FetchCycles*10 + scanlineEndCycles);
	}
	updateZapper();

	nesPpu.nextScanline();
	for (; nesPpuScanline < 240; nesPpu.nextScanline()) {
		if (!pre)
			clockCpu(all ? scanlineCycles : hDrawCycles);
		if (drawEnabled) {
			nesPpu.processScanline();
		} else {
			if (nesPad.isZapperMode() && nesPpuScanline == ZapperY ) {
				nesPpu.processScanline();
			} else {
				if (nesPpu.checkSprite0HitHere())
					nesPpu.processScanline();
				else
					nesPpu.processDummyScanline();
			}
		}
		if (all) {
			nesPpu.processScanlineNext();
			if (pre)
				clockCpu(scanlineCycles);
			nesMapper->horizontalSync();
			nesPpu.processScanlineStart();
		} else {
			if (pre)
				clockCpu(hDrawCycles);
			nesPpu.processScanlineNext();
			nesMapper->horizontalSync();
			clockCpu(NesPpu::FetchCycles*32);
			nesPpu.processScanlineStart();
			clockCpu(NesPpu::FetchCycles*10 + scanlineEndCycles);
		}
		updateZapper();
	}

	nesMapper->verticalSync();
	if (all) {
		clockCpu(scanlineCycles);
		nesMapper->horizontalSync();
	} else {
		clockCpu(hDrawCycles);
		nesMapper->horizontalSync();
		clockCpu(hBlankCycles);
	}
	updateZapper();

	nesPpu.nextScanline();
	int totalScanlines = nesPpuScanlinesPerFrame;
	for (; nesPpuScanline <= totalScanlines-1; nesPpu.nextScanline()) {
		if (nesPpuScanline == 241)
			nesPpu.setVBlank(true);
		else if (nesPpuScanline == totalScanlines-1)
			nesPpu.setVBlank(false);

		if (all) {
			clockCpu(scanlineCycles);
			nesMapper->horizontalSync();
		} else {
			clockCpu(hDrawCycles);
			nesMapper->horizontalSync();
			clockCpu(hBlankCycles);
		}

		if (nesPpuScanline != totalScanlines-1)
			updateZapper();
	}
}

inline void NesMachine::emulateVisibleScanlineTile() {
	nesPpu.processScanlineNext();
	clockCpu(NesPpu::FetchCycles*10);
	nesMapper->horizontalSync();
	clockCpu(NesPpu::FetchCycles*22);
	nesPpu.processScanlineStart();
	clockCpu(NesPpu::FetchCycles*10 + scanlineEndCycles);
	updateZapper();
}

void NesMachine::emulateFrameTile(bool drawEnabled) {
	clockCpu(NesPpu::FetchCycles*128);
	nesPpu.processFrameStart();
	emulateVisibleScanlineTile();

	if (drawEnabled) {
		nesPpu.nextScanline();
		for (; nesPpuScanline < 240; nesPpu.nextScanline()) {
			nesPpu.processScanline();
			emulateVisibleScanlineTile();
		}
	} else {
		for (; nesPpuScanline < 240; nesPpu.nextScanline()) {
			if (nesPad.isZapperMode() && nesPpuScanline == ZapperY)
				nesPpu.processScanline();
			else {
				if (nesPpu.checkSprite0HitHere()) {
					nesPpu.processScanline();
				} else {
					clockCpu(NesPpu::FetchCycles*128);
					nesPpu.processDummyScanline();
				}
			}
			emulateVisibleScanlineTile();
		}
	}

	nesMapper->verticalSync();
	clockCpu(hDrawCycles);
	nesMapper->horizontalSync();
	clockCpu(hBlankCycles);
	updateZapper();

	nesPpu.nextScanline();
	int totalScanlines = nesPpuScanlinesPerFrame;
	for (; nesPpuScanline <= totalScanlines-1; nesPpu.nextScanline()) {
		if (nesPpuScanline == 241)
			nesPpu.setVBlank(true);
		else if (nesPpuScanline == totalScanlines-1)
			nesPpu.setVBlank(false);

		clockCpu(hDrawCycles);
		nesMapper->horizontalSync();
		clockCpu(hBlankCycles);

		if (nesPpuScanline != totalScanlines-1)
			updateZapper();
	}
}

int NesMachine::fillAudioBuffer(char *stream, int streamSize)
{ return nesApu.fillBuffer(stream, streamSize); }

NesPpu *NesMachine::ppu() const
{ return &nesPpu; }

void NesMachine::sl() {
	nesMapper->sl();
	nesCpu.sl();
	nesPpu.sl();
	nesApu.sl();

	emsl.begin("machine");
	emsl.var("cpuCycleCounter", cpuCycleCounter);
	emsl.var("ppuCycleCounter", ppuCycleCounter);
	emsl.end();
}

int main(int argc, char *argv[]) {
	if (argc < 2)
		return -1;
	QApplication app(argc, argv);
	// TODO machine on stack
	MachineView view(new NesMachine(), argv[1]);
	GameGenieCodeListModel gameGenie(static_cast<NesMachine *>(view.machine()));
	view.settingsView()->rootContext()->setContextProperty("gameGenie", static_cast<QObject *>(&gameGenie));
	return app.exec();
}
