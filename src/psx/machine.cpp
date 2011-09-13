#include "machine.h"
#include <QString>
#include <QImage>
#include <QPainter>
#include <QApplication>
#include <QTimer>
#include <QFileInfo>
#include <machineview.h>
#include <sys/time.h>

#include "psxcommon.h"
#include "cdrom.h"
#include "plugins.h"
#include "sio.h"
#include "misc.h"
#include "r3000a.h"
#include "gpu/newGPU.h"

#define CONFIG_DIR		"/home/user/MyDocs/emumaster/psx"

PsxMachine psxMachine;
PsxThread psxThread;
QImage gpuFrame;

extern int stop;
extern BOOL enableAbbeyHack;

unsigned long timeGetTime() {
 struct timeval tv;
 gettimeofday(&tv, 0);                                 // well, maybe there are better ways
 return tv.tv_sec * 1000 + tv.tv_usec/1000;            // to do that, but at least it works
}

extern "C" void qDebugC(const char *msg) {
	qDebug(msg);
}

static void emu_config() {
	Config.UseNet = 0;
	Config.HLE = 0;
	strcpy(Config.Net, _("Disabled"));

	strcpy(Config.Mcd1, CONFIG_DIR"/mcd001.mcr");
	strcpy(Config.Mcd2, CONFIG_DIR"/mcd002.mcr");
	Config.PsxAuto = 1;
	Config.Cdda = 0;
	Config.Xa = 0;
#ifdef DYNAREC
	Config.Cpu = CPU_DYNAREC;
#else
 	Config.Cpu = CPU_INTERPRETER;
#endif
	Config.Mdec = 0;
	Config.PsxOut = 0;
	Config.PsxType = 0;
	Config.RCntFix = 0;
	Config.Sio = 0;
	Config.SpuIrq = 0;
	Config.VSyncWA = 0;
	strcpy(Config.Bios, "scph1001.bin");
	strcpy(Config.BiosDir, "/home/user/MyDocs/emumaster/psx");
}

PsxMachine::PsxMachine(QObject *parent) :
	IMachine("psx", parent) {
	m_quit = false;
	stop = 0;
	emu_config();
	SysInit();
	setVideoSrcRect(QRect(0, 0, 256, 240));
	setFrameRate(50); // TODO PAL/NTSC
}

void PsxMachine::reset() {
	SysReset();
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
		if (LoadPlugins() == -1)
			return "Could not load plugins.";
		CheckCdrom();
		SysReset();
		if (LoadCdrom() == -1) {
			SysClose();
			return "Could not load CD.";
		}
		Skip = 1;
		psxThread.start();
		m_consSem.acquire();
	}
	return QString();
}

void PsxMachine::emulateFrame(bool drawEnabled) {
	Skip = drawEnabled;
	m_prodSem.release();
	m_consSem.acquire();
}

const QImage &PsxMachine::frame() const
{ return gpuFrame; }

extern int spuFillBuffer(char *stream, int length);

int PsxMachine::fillAudioBuffer(char *stream, int streamSize)
{ return spuFillBuffer(stream, streamSize); }

void PsxMachine::setPadKey(PadKey key, bool state) {
}

void PsxThread::run() {
	psxCpu->Execute();
	SysClose();
}

#define STATE_SERIALIZE_BUILDER(sl) \
	STATE_SERIALIZE_BEGIN_##sl(PsxMachine, 1) \
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
