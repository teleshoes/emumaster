#include "machine.h"
#include <QString>
#include <QImage>
#include <QPainter>
#include <QApplication>
#include <QTimer>
#include <QFileInfo>
#include <machineview.h>
#include <sys/time.h>

#include "common.h"
#include "cdrom.h"
#include "misc.h"
#include "mdec.h"
#include "sio.h"
#include "ppf.h"
#include "cpu_int.h"
#include "cpu_rec.h"
#include "gpu_unai.h"
#include "spu_null.h"
#include "cdriso.h"

PcsxConfig Config;

static void dummy_lace() { }

#define CONFIG_DIR		"/home/user/MyDocs/emumaster/psx"

PsxMachine psxMachine;
PsxThread psxThread;

PsxMachine::PsxMachine(QObject *parent) :
	IMachine("psx", parent) {
}

QString PsxMachine::init() {
	m_quit = false;

	Config.HLE = 0;

	psxMcd1.init(userDataDirPath() + "/psx_mcd1.mcr");
	psxMcd2.init(userDataDirPath() + "/psx_mcd2.mcr");
	Config.PsxAuto = 1;
	Config.Cdda = 0;
	Config.Xa = 0;
	Config.Mdec = 0;
	Config.PsxOut = 0;
	Config.RCntFix = 0;
	Config.Sio = 0;
	Config.SpuIrq = 0;
	Config.VSyncWA = 0;
	// TODO give user ability to choose his bios
	psxMem.setBiosName("scph1001.bin");

	systemType = NtscType;
	setVideoSrcRect(QRect(0, 0, 256, 240));
	setFrameRate(60); // TODO PAL/NTSC

	// TODO option to set interpreter cpu
//	if (Config.Cpu == CpuInterpreter)
//		psxCpu = &psxInt;
//	else
		psxCpu = &psxRec;

		psxGpu = &psxGpuUnai;
		psxSpu = &psxSpuNull;

	if (!psxMemInit())
		return "Could not allocate memory!";
	if (!psxCpu->init())
		return "Could not initialize CPU!";

	// TODO debug as preprocessor
	if (Config.Debug) {
		StartDebugger();
	}

	cdrIsoInit();
	if (CDR_init() < 0)
		return "Could not initialize CD-ROM!";;
	if (!psxGpu->init())
		return "Could not initialize GPU!";
	if (!psxSpu->init())
		return "Could not initialize SPU!";
	return QString();
}

void PsxMachine::shutdown() {
	FreePPFCache();
	psxMemShutdown();
	psxCpu->shutdown();

	StopDebugger();
}

void PsxMachine::reset() {
	// rearmed hack: reset runs some code when real BIOS is used,
	// but we usually do reset from menu while GPU is not open yet,
	// so we need to prevent updateLace() call..
	void (*real_lace)() = GPU_updateLace;
	GPU_updateLace = dummy_lace;

	psxMemReset();

	memset(&psxRegs, 0, sizeof(psxRegs));
	psxRegs.pc = 0xBFC00000; // Start in bootstrap
	psxRegs.CP0.r[12] = 0x10900000; // COP0 enabled | BEV = 1 | TS = 1
	psxRegs.CP0.r[15] = 0x00000002; // PRevID = Revision ID, same as R3000A

	psxCpu->reset();

	psxHwReset();
	psxBiosInit();

	if (!Config.HLE) {
		// skip bios logo TODO option
		while (psxRegs.pc != 0x80030000)
			psxCpu->executeBlock();
	}

	// hmh core forgets this
	CDR_stop();

	GPU_updateLace = real_lace;

	CheckCdrom();
	LoadCdrom();
}

void PsxMachine::updateGpuScale(int w, int h) {
	setVideoSrcRect(QRect(0, 0, w, h));
}

void PsxMachine::flipScreen() {
	m_consSem.release();
	// TODO rework
	if (m_quit)
		psxThread.terminate();
	m_prodSem.acquire();
}

QString PsxMachine::setDisk(const QString &path) {
	SetCdOpenCaseTime(time(0) + 2);
	QFileInfo fi;
	fi.setFile(path + ".iso");
	if (!fi.exists()) {
		fi.setFile(path + ".bin");
	}
	SetIsoFile(fi.filePath().toAscii().constData());
	if (!psxThread.isRunning()) {
		if (CDR_open() < 0)
			return "Could not open CD-ROM.";
		CheckCdrom();
		reset();
		if (LoadCdrom() == -1)
			return "Could not load CD.";
		psxThread.start();
		m_consSem.acquire();
	}
	return QString();
}

void PsxMachine::emulateFrame(bool drawEnabled) {
	psxGpu->setDrawEnabled(drawEnabled);
	m_prodSem.release();
	m_consSem.acquire();
}

const QImage &PsxMachine::frame() const
{ return psxGpu->frame(); }

int PsxMachine::fillAudioBuffer(char *stream, int streamSize)
{ // TODO
	return 0;
}

extern void setPadButtons(int machineKeys);

void PsxMachine::setPadKeys(int pad, int keys) {
	if (pad)
		return;
	// TODO many pads
	setPadButtons(keys);
}

void PsxThread::run() {
	psxCpu->execute();
}

#define STATE_SERIALIZE_BUILDER(sl) \
STATE_SERIALIZE_BEGIN_##sl(PsxMachine, 1) \
	STATE_SERIALIZE_VAR_##sl(Config.HLE) \
	STATE_SERIALIZE_VAR_##sl(systemType) \
	if (STATE_SERIALIZE_TEST_TYPE_##sl) { \
		new_dyna_save(); \
		if (Config.HLE) \
			psxBiosFreeze(1); \
	} else { \
		if (Config.HLE) \
			psxBiosInit(); \
	} \
	STATE_SERIALIZE_ARRAY_##sl(psxM, 0x00200000) \
	STATE_SERIALIZE_ARRAY_##sl(psxR, 0x00080000) \
	STATE_SERIALIZE_ARRAY_##sl(psxH, 0x00010000) \
	STATE_SERIALIZE_ARRAY_##sl(&psxRegs, sizeof(psxRegs)) \
	if (!STATE_SERIALIZE_TEST_TYPE_##sl) { \
		if (Config.HLE) \
			psxBiosFreeze(0); \
		psxCpu->reset(); \
	} \
	STATE_SERIALIZE_SUBCALL_##sl(psxSio) \
	STATE_SERIALIZE_SUBCALL_##sl(psxCdr) \
	STATE_SERIALIZE_SUBCALL_##sl(psxCnt) \
	STATE_SERIALIZE_SUBCALL_##sl(psxMdec) \
	STATE_SERIALIZE_SUBCALL_PTR_##sl(psxGpu) \
	STATE_SERIALIZE_SUBCALL_PTR_##sl(psxSpu) \
	if (!STATE_SERIALIZE_TEST_TYPE_##sl) { \
		new_dyna_restore(); \
	} \
STATE_SERIALIZE_END_##sl(PsxMachine)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)

int main(int argc, char *argv[]) {
	if (argc < 2)
		return -1;
	QApplication app(argc, argv);
	MachineView view(&psxMachine, argv[1]);
	return app.exec();
}
