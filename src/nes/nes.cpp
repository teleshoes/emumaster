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
static int totalScanlines;

static u64 cpuCycleCounter;
static u64 ppuCycleCounter;

static const char *tvSystemConfName = "nes.tvSystem";
static const char *renderMethodConfName = "nes.renderMethod";

static QVariant forcedRenderMethod;

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
	bool ok = false;
	// check if TV system used in the saved state is the same as current
	QVariant forcedSystemType = emConf.value(tvSystemConfName);
	if (!forcedSystemType.isNull()) {
		QString typeStr = forcedSystemType.toString();
		SystemType slSys = (typeStr == "PAL") ? NES_PAL : NES_NTSC;
		ok = (slSys == nesSystemType);
	}
	if (!ok) {
		emsl.error = QString("%1 \"%2\"").arg(EM_MSG_STATE_DIFFERS)
				.arg(tvSystemConfName);
	}
	return ok;
}

static void setupRenderMethod()
{
	nesEmuSetRenderMethod(NesEmu::PreRender);
	// check for forced tv system
	forcedRenderMethod = emConf.value(renderMethodConfName);
	if (!forcedRenderMethod.isNull()) {
		bool ok;
		int method = forcedRenderMethod.toInt(&ok);
		ok = (ok && method >= NesEmu::PostAllRender && method <= NesEmu::TileRender);
		if (!ok) {
			qDebug("Invalid render method passed");
			forcedRenderMethod = QVariant();
		} else {
			nesEmuRenderMethod = static_cast<NesEmu::RenderMethod>(method);
		}
	}
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
	nesPpuProcessFrameStart();
	nesPpuProcessScanlineNext();
	nesMapper->horizontalSync();
	if (all) {
		nesPpuProcessScanlineStart();
	} else {
		nesEmuClockCpu(NesPpu::FetchCycles*32);
		nesPpuProcessScanlineStart();
		nesEmuClockCpu(NesPpu::FetchCycles*10 + scanlineEndCycles);
	}
	updateZapper();

	nesPpuNextScanline();
	for (; nesPpuScanline < 240; nesPpuNextScanline()) {
		if (!pre)
			nesEmuClockCpu(all ? scanlineCycles : hDrawCycles);
		if (drawEnabled || nesPpuCheckSprite0HitHere())
			nesPpuProcessScanline();
		else
			nesPpuProcessDummyScanline();
		if (all) {
			nesPpuProcessScanlineNext();
			if (pre)
				nesEmuClockCpu(scanlineCycles);
			nesMapper->horizontalSync();
			nesPpuProcessScanlineStart();
		} else {
			if (pre)
				nesEmuClockCpu(hDrawCycles);
			nesPpuProcessScanlineNext();
			nesMapper->horizontalSync();
			nesEmuClockCpu(NesPpu::FetchCycles*32);
			nesPpuProcessScanlineStart();
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

	nesPpuNextScanline();
	for (; nesPpuScanline <= totalScanlines-1; nesPpuNextScanline()) {
		if (nesPpuScanline == 241)
			nesPpuSetVBlank(true);
		else if (nesPpuScanline == totalScanlines-1)
			nesPpuSetVBlank(false);

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
	nesPpuProcessScanlineNext();
	nesEmuClockCpu(NesPpu::FetchCycles*10);
	nesMapper->horizontalSync();
	nesEmuClockCpu(NesPpu::FetchCycles*22);
	nesPpuProcessScanlineStart();
	nesEmuClockCpu(NesPpu::FetchCycles*10 + scanlineEndCycles);
	updateZapper();
}

static void emulateFrameTile(bool drawEnabled)
{
	nesEmuClockCpu(NesPpu::FetchCycles*128);
	nesPpuProcessFrameStart();
	emulateVisibleScanlineTile();

	for (; nesPpuScanline < 240; nesPpuNextScanline()) {
		if (drawEnabled || nesPpuCheckSprite0HitHere()) {
			nesPpuProcessScanline();
		} else {
			nesEmuClockCpu(NesPpu::FetchCycles*128);
			nesPpuProcessDummyScanline();
		}
		emulateVisibleScanlineTile();
	}

	nesMapper->verticalSync();
	nesEmuClockCpu(hDrawCycles);
	nesMapper->horizontalSync();
	nesEmuClockCpu(hBlankCycles);
	updateZapper();

	nesPpuNextScanline();
	for (; nesPpuScanline <= totalScanlines-1; nesPpuNextScanline()) {
		if (nesPpuScanline == 241)
			nesPpuSetVBlank(true);
		else if (nesPpuScanline == totalScanlines-1)
			nesPpuSetVBlank(false);

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

bool NesEmu::init(const QString &diskPath, QString *error)
{
	qmlRegisterType<GameGenieValidator>("EmuMaster", 1, 0, "GameGenieValidator");
	setupRenderMethod();

	if (!nesDiskLoad(diskPath, error)) {
		*error = QString("%1 (%2)").arg(EM_MSG_DISK_LOAD_FAILED).arg(*error);
		return false;
	}

	setupTvEncodingSystem(diskPath);

	nesMapper = NesMapper::create(nesMapperType);
	if (!nesMapper) {
		*error = QString("Mapper %1 is not supported").arg(nesMapperType);
		return false;
	}

	setVideoSrcRect(QRect(8, 0, NesPpu::VisibleScreenWidth, NesPpu::VisibleScreenHeight));
	setupTimings();

	nesCpu.init();
	nesApuInit();
	nesPpuInit();
	reset();
	return true;
}

void NesEmu::setupTimings()
{	
	if (nesSystemType == NES_NTSC) {
		totalScanlines = 262;
		setFrameRate(NES_NTSC_FRAMERATE);
		scanlineCycles = NES_NTSC_SCANLINE_CLOCKS;
		hDrawCycles = 1024;
		hBlankCycles = 340;
		scanlineEndCycles = 4;
	} else {
		totalScanlines = 312;
		setFrameRate(NES_PAL_FRAMERATE);
		scanlineCycles = NES_PAL_SCANLINE_CLOCKS;
		hDrawCycles = 960;
		hBlankCycles = 318;
		scanlineEndCycles = 2;
	}
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

u64 nesEmuCpuCycles()
{
	return cpuCycleCounter + nesCpu.ticks();
}

void nesEmuSetRenderMethod(NesEmu::RenderMethod renderMethod)
{
	if (forcedRenderMethod.isNull())
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
	nesPpuSl();
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
