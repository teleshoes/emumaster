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

#include "nes.h"
#include "cpu.h"
#include "ppu.h"
#include "apu.h"
#include "disk.h"
#include "input.h"
#include "mapper.h"
#include "cheats.h"
#include "timings.h"
#include <emuview.h>
#include <configuration.h>
#include <QSettings>
#include <QApplication>
#include <qdeclarative.h>

NesEmu nesEmu;
SystemType nesSystemType;
static NesEmu::RenderMethod nesRenderMethod;
NesCheats nesCheats;

static u32 scanlineCycles;
static u32 scanlineEndCycles;
static u32 hDrawCycles;
static u32 hBlankCycles;

static u64 cpuCycleCounter;
static u64 ppuCycleCounter;

static bool bZapper; // TODO zapper
static int ZapperY;

static const char *tvSystemConfName = "nes.tvSystem";
static const char *renderMethodConfName = "nes.renderMethod";

NesEmu::NesEmu() :
	Emu("nes")
{
}

QString NesEmu::init(const QString &diskPath)
{
	qmlRegisterType<GameGenieValidator>("EmuMaster", 1, 0, "GameGenieValidator");
	QString error = setDisk(diskPath);
	if (!error.isEmpty())
		return error;
	setRenderMethod(PreRender);
	nesCpu.init();
	nesApuInit();
	nesPadInit();
	nesPpu.init();
	return QString();
}

void NesEmu::shutdown()
{
	delete nesMapper;
	delete nesVrom;
	delete nesRom;
	nesPpuFrame = QImage();
}

void NesEmu::reset()
{
	cpuCycleCounter = 0;
	ppuCycleCounter = 0;
	nesMapper->reset();
	nesCpu.reset();
	nesApuReset();
}

void NesEmu::resume()
{
	nesApuSetOutputEnabled(isAudioEnabled());
}

QString NesEmu::setDisk(const QString &path)
{
	QString loadError;
	if (!nesDiskLoad(path, &loadError)) {
		return QString("%1 (%2)").arg(EM_MSG_DISK_LOAD_FAILED)
				.arg(loadError);
	}

	setupTvEncodingSystem(path);

	nesMapper = NesMapper::create(nesMapperType);
	if (!nesMapper)
		return QString("Mapper %1 is not supported").arg(nesMapperType);

	// TODO VS system
	if (nesSystemType == NES_NTSC) {
		setVideoSrcRect(QRect(8, 8, NesPpu::VisibleScreenWidth-8, NesPpu::VisibleScreenHeight-16));
		setFrameRate(NES_NTSC_FRAMERATE);
		scanlineCycles = NES_NTSC_SCANLINE_CLOCKS;
		hDrawCycles = 1024;
		hBlankCycles = 340;
		scanlineEndCycles = 4;
	} else {
		setVideoSrcRect(QRect(8, 1, NesPpu::VisibleScreenWidth-8, NesPpu::VisibleScreenHeight-1));
		setFrameRate(NES_PAL_FRAMERATE);
		scanlineCycles = NES_PAL_SCANLINE_CLOCKS;
		hDrawCycles = 960;
		hBlankCycles = 318;
		scanlineEndCycles = 2;
	}
	reset();
	return QString();
}

NesEmu::RenderMethod NesEmu::renderMethod() const
{
	return nesRenderMethod;
}

void NesEmu::setRenderMethod(RenderMethod method)
{
	nesRenderMethod = method;
}

void NesEmu::setupTvEncodingSystem(const QString &path)
{
	// detect system name by checking file name
	if (path.contains("(E)"))
		nesSystemType = NES_PAL;
	else
		nesSystemType = NES_NTSC;

	// check for forced tv system
	QVariant forcedSystemType = emConf.value(tvSystemConfName);
	if (!forcedSystemType.isNull()) {
		QString typeStr = forcedSystemType.toString();
		if (typeStr == "NTSC")
			nesSystemType = NES_NTSC;
		else if (typeStr == "PAL")
			nesSystemType = NES_PAL;
		else
			qDebug("Unknown TV system set: %s", qPrintable(typeStr));
	}
	// set value in conf in case of auto-detection (needed for load check)
	if (nesSystemType == NES_NTSC)
		emConf.setValue(tvSystemConfName, "NTSC");
	else
		emConf.setValue(tvSystemConfName, "PAL");
}

bool NesEmu::slCheckTvEncodingSystem() const
{
	// TODO add it in state converter
	bool ok = false;
	// check if TV system used in the saved state is the same as current
	QVariant forcedSystemType = emConf.value(tvSystemConfName);
	if (!forcedSystemType.isNull()) {
		QString typeStr = forcedSystemType.toString();
		bool slSys = (typeStr == "NTSC");
		bool emuSys = (nesSystemType == NES_NTSC);
		ok = (slSys == emuSys);
	}
	if (!ok) {
		emsl.error = QString("%1 \"%2\"").arg(EM_MSG_STATE_DIFFERS)
				.arg(tvSystemConfName);
	}
	return ok;
}

void NesEmu::clockCpu(uint cycles)
{
	ppuCycleCounter += cycles;
	int realCycles = (ppuCycleCounter/12) - cpuCycleCounter;
	if (realCycles > 0)
		cpuCycleCounter += nesCpu.clock(realCycles);
}

const QImage &NesEmu::frame() const
{
	return nesPpuFrame;
}

void NesEmu::emulateFrame(bool drawEnabled)
{
	nesPadSetButtons(0, input()->pad[0].buttons());
	nesPadSetButtons(1, input()->pad[1].buttons());

	nesApuBeginFrame();
	bZapper = false;
	if (nesEmu.renderMethod() == NesEmu::TileRender)
		emulateFrameTile(drawEnabled);
	else
		emulateFrameNoTile(drawEnabled);
	nesApuProcessFrame();
}

inline void NesEmu::updateZapper()
{
	if (nesPadIsZapperMode())
		bZapper = (nesPpuScanline == ZapperY);
}

void NesEmu::emulateFrameNoTile(bool drawEnabled)
{
	RenderMethod renderMethod = nesRenderMethod;
	bool all = (renderMethod < NesEmu::PostRender);
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
			if (nesPadIsZapperMode() && nesPpuScanline == ZapperY ) {
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

inline void NesEmu::emulateVisibleScanlineTile()
{
	nesPpu.processScanlineNext();
	clockCpu(NesPpu::FetchCycles*10);
	nesMapper->horizontalSync();
	clockCpu(NesPpu::FetchCycles*22);
	nesPpu.processScanlineStart();
	clockCpu(NesPpu::FetchCycles*10 + scanlineEndCycles);
	updateZapper();
}

void NesEmu::emulateFrameTile(bool drawEnabled)
{
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
			if (nesPadIsZapperMode() && nesPpuScanline == ZapperY)
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

int NesEmu::fillAudioBuffer(char *stream, int streamSize)
{
	return nesApuFillBuffer(stream, streamSize);
}

NesPpu *NesEmu::ppu() const
{
	return &nesPpu;
}

QObject *NesEmu::cheats() const
{
	return &nesCheats;
}

u64 NesEmu::cpuCycles() const
{
	return cpuCycleCounter + nesCpu.ticks();
}

void NesEmu::sl()
{
	if (!slCheckTvEncodingSystem())
		return;
	nesMapper->sl();
	nesCpu.sl();
	nesPpu.sl();
	nesApuSl();
	nesCheats.sl();

	emsl.begin("machine");
	emsl.var("cpuCycleCounter", cpuCycleCounter);
	emsl.var("ppuCycleCounter", ppuCycleCounter);
	emsl.end();
}

int main(int argc, char *argv[])
{
	if (argc < 2)
		return -1;
	QApplication app(argc, argv);
	qmlRegisterType<NesPpu>();
	EmuView view(&nesEmu, argv[1]);
	return app.exec();
}
