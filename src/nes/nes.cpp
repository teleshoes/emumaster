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
#include "inputzapper.h"
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
NesEmu::RenderMethod nesEmuRenderMethod;

static u32 scanlineCycles;
static u32 scanlineEndCycles;
static u32 hDrawCycles;
static u32 hBlankCycles;

static u64 cpuCycleCounter;
static u64 ppuCycleCounter;

static const char *tvSystemConfName = "nes.tvSystem";
static const char *renderMethodConfName = "nes.renderMethod";

static void setupTvEncodingSystem(const QString &path)
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

static bool slCheckTvEncodingSystem()
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

void nesEmuClockCpu(uint cycles)
{
	ppuCycleCounter += cycles;
	int realCycles = (ppuCycleCounter/12) - cpuCycleCounter;
	if (realCycles > 0)
		cpuCycleCounter += nesCpu.clock(realCycles);
}

static inline void updateZapper()
{
	nesInputZapper.setScanlineHit(nesPpuScanline == nesInputZapper.pos().y());
}

static void emulateFrameNoTile(bool drawEnabled)
{
	bool all = (nesEmuRenderMethod < NesEmu::PostRender);
	bool pre = (nesEmuRenderMethod & 1);

	nesEmuClockCpu(all ? scanlineCycles : hDrawCycles);
	nesPpu.processFrameStart();
	nesPpu.processScanlineNext();
	nesMapper->horizontalSync();
	if (all) {
		nesPpu.processScanlineStart();
	} else {
		nesEmuClockCpu(NesPpu::FetchCycles*32);
		nesPpu.processScanlineStart();
		nesEmuClockCpu(NesPpu::FetchCycles*10 + scanlineEndCycles);
	}
	updateZapper();

	nesPpu.nextScanline();
	for (; nesPpuScanline < 240; nesPpu.nextScanline()) {
		if (!pre)
			nesEmuClockCpu(all ? scanlineCycles : hDrawCycles);
		if (drawEnabled || nesPpu.checkSprite0HitHere())
			nesPpu.processScanline();
		else
			nesPpu.processDummyScanline();
		if (all) {
			nesPpu.processScanlineNext();
			if (pre)
				nesEmuClockCpu(scanlineCycles);
			nesMapper->horizontalSync();
			nesPpu.processScanlineStart();
		} else {
			if (pre)
				nesEmuClockCpu(hDrawCycles);
			nesPpu.processScanlineNext();
			nesMapper->horizontalSync();
			nesEmuClockCpu(NesPpu::FetchCycles*32);
			nesPpu.processScanlineStart();
			nesEmuClockCpu(NesPpu::FetchCycles*10 + scanlineEndCycles);
		}
		updateZapper();
	}

	nesMapper->verticalSync();
	if (all) {
		nesEmuClockCpu(scanlineCycles);
		nesMapper->horizontalSync();
	} else {
		nesEmuClockCpu(hDrawCycles);
		nesMapper->horizontalSync();
		nesEmuClockCpu(hBlankCycles);
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
			nesEmuClockCpu(scanlineCycles);
			nesMapper->horizontalSync();
		} else {
			nesEmuClockCpu(hDrawCycles);
			nesMapper->horizontalSync();
			nesEmuClockCpu(hBlankCycles);
		}

		if (nesPpuScanline != totalScanlines-1)
			updateZapper();
	}
}

static inline void emulateVisibleScanlineTile()
{
	nesPpu.processScanlineNext();
	nesEmuClockCpu(NesPpu::FetchCycles*10);
	nesMapper->horizontalSync();
	nesEmuClockCpu(NesPpu::FetchCycles*22);
	nesPpu.processScanlineStart();
	nesEmuClockCpu(NesPpu::FetchCycles*10 + scanlineEndCycles);
	updateZapper();
}

static void emulateFrameTile(bool drawEnabled)
{
	nesEmuClockCpu(NesPpu::FetchCycles*128);
	nesPpu.processFrameStart();
	emulateVisibleScanlineTile();

	for (; nesPpuScanline < 240; nesPpu.nextScanline()) {
		if (drawEnabled || nesPpu.checkSprite0HitHere()) {
			nesPpu.processScanline();
		} else {
			nesEmuClockCpu(NesPpu::FetchCycles*128);
			nesPpu.processDummyScanline();
		}
		emulateVisibleScanlineTile();
	}

	nesMapper->verticalSync();
	nesEmuClockCpu(hDrawCycles);
	nesMapper->horizontalSync();
	nesEmuClockCpu(hBlankCycles);
	updateZapper();

	nesPpu.nextScanline();
	int totalScanlines = nesPpuScanlinesPerFrame;
	for (; nesPpuScanline <= totalScanlines-1; nesPpu.nextScanline()) {
		if (nesPpuScanline == 241)
			nesPpu.setVBlank(true);
		else if (nesPpuScanline == totalScanlines-1)
			nesPpu.setVBlank(false);

		nesEmuClockCpu(hDrawCycles);
		nesMapper->horizontalSync();
		nesEmuClockCpu(hBlankCycles);

		if (nesPpuScanline != totalScanlines-1)
			updateZapper();
	}
}

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
	// TODO setup rendering
	nesEmuSetRenderMethod(PreRender);
	nesCpu.init();
	nesApuInit();
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
	nesInputReset();
}

void NesEmu::resume()
{
	nesApuSetOutputEnabled(isAudioEnabled());
}

QString NesEmu::setDisk(const QString &path)
{
	QString loadError;
	if (!nesDiskLoad(path, &loadError))
		return QString("%1 (%2)").arg(EM_MSG_DISK_LOAD_FAILED).arg(loadError);

	setupTvEncodingSystem(path);

	nesMapper = NesMapper::create(nesMapperType);
	if (!nesMapper)
		return QString("Mapper %1 is not supported").arg(nesMapperType);

	// TODO VS system
	setVideoSrcRect(QRect(8, 0, NesPpu::VisibleScreenWidth, NesPpu::VisibleScreenHeight));
	if (nesSystemType == NES_NTSC) {
		setFrameRate(NES_NTSC_FRAMERATE);
		scanlineCycles = NES_NTSC_SCANLINE_CLOCKS;
		hDrawCycles = 1024;
		hBlankCycles = 340;
		scanlineEndCycles = 4;
	} else {
		setFrameRate(NES_PAL_FRAMERATE);
		scanlineCycles = NES_PAL_SCANLINE_CLOCKS;
		hDrawCycles = 960;
		hBlankCycles = 318;
		scanlineEndCycles = 2;
	}
	reset();
	return QString();
}

u64 nesEmuCpuCycles()
{
	return cpuCycleCounter + nesCpu.ticks();
}

void nesEmuSetRenderMethod(NesEmu::RenderMethod renderMethod)
{
	// TODO setup render method and return if force
	nesEmuRenderMethod = renderMethod;
}

void NesEmu::emulateFrame(bool drawEnabled)
{
	nesInputSync(input());
	nesApuBeginFrame();
	// zapper is rarely used, but once used it forces drawing
	drawEnabled |= (nesInput.extraDevice() == NesInput::Zapper);
	if (nesEmuRenderMethod == TileRender)
		emulateFrameTile(drawEnabled);
	else
		emulateFrameNoTile(drawEnabled);
	nesApuProcessFrame();
}

const QImage &NesEmu::frame() const
{
	return nesPpuFrame;
}

int NesEmu::fillAudioBuffer(char *stream, int streamSize)
{
	return nesApuFillBuffer(stream, streamSize);
}

QObject *NesEmu::ppu() const
{
	return &nesPpu;
}

QObject *NesEmu::pad() const
{
	return &nesInput;
}

QObject *NesEmu::cheats() const
{
	return &nesCheats;
}

void NesEmu::sl()
{
	if (!slCheckTvEncodingSystem())
		return;
	nesMapper->sl();
	nesCpu.sl();
	nesPpu.sl();
	nesApuSl();
	nesInput.sl();
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
