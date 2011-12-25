#include "machine.h"
#include "machineview.h"
#include "pico.h"
#include "cart.h"
#include "maemo.h"
#include "pathmanager.h"
#include <QImage>
#include <QApplication>
#include <QFile>
#include <QDir>

PicoMachine picoMachine;
QImage picoFrame;

PicoMachine::PicoMachine() :
	IMachine("pico")
{
}

static int modes = 0;

#define SCREEN_WIDTH 320

void *md_screen = 0;
unsigned char *PicoDraw2FB = 0;  // temporary buffer for alt renderer

static void cd_leds() {
//	static
	int old_reg;
//	if (!((Pico_mcd->s68k_regs[0] ^ old_reg) & 3)) return; // no change // mmu hack problems?
	old_reg = Pico_mcd->s68k_regs[0];

	// 16-bit modes
	unsigned int *p = (unsigned int *)((short *)md_screen + SCREEN_WIDTH*2+4);
	unsigned int col_g = (old_reg & 2) ? 0x06000600 : 0;
	unsigned int col_r = (old_reg & 1) ? 0xc000c000 : 0;
	*p++ = col_g; *p++ = col_g; p+=2; *p++ = col_r; *p++ = col_r; p += SCREEN_WIDTH/2 - 12/2;
	*p++ = col_g; *p++ = col_g; p+=2; *p++ = col_r; *p++ = col_r; p += SCREEN_WIDTH/2 - 12/2;
	*p++ = col_g; *p++ = col_g; p+=2; *p++ = col_r; *p++ = col_r;
}

void picoScanLine(uint num) {
	DrawLineDest = (u16 *)md_screen + SCREEN_WIDTH*(num+1);
}

bool PicoMachine::findMcdBios(QString *biosFileName, QString *error)
{
	QStringList possibleBiosNames;
	int region = Pico_mcd->TOC.region();

	if (region == PicoRegionUsa) {
		possibleBiosNames << "us_scd2_9306.bin";
		possibleBiosNames << "us_scd2_9303.bin";
		possibleBiosNames << "us_scd1_9210.bin";
	} else if (region == PicoRegionEurope) {
		possibleBiosNames << "eu_mcd2_9306.bin";
		possibleBiosNames << "eu_mcd2_9303.bin";
		possibleBiosNames << "eu_mcd1_9210.bin";
	} else if (region == PicoRegionJapanNtsc || region == PicoRegionJapanPal) {
		possibleBiosNames << "jp_mcd1_9112.bin";
		possibleBiosNames << "jp_mcd1_9111.bin";
	} else {
		Q_ASSERT(false);
		*error = QObject::tr("Internal error");
	}

	*biosFileName = QString();
	QDir dir(PathManager::instance()->diskDirPath());
	foreach (QString name, possibleBiosNames) {
		if (dir.exists(name)) {
			*biosFileName = dir.filePath(name);
			break;
		}
	}
	if (biosFileName->isEmpty()) {
		*error = QObject::tr("BIOS file not found");
		return false;
	}

	// the bios must be of size 128KB
	QFileInfo biosFileInfo(*biosFileName);
	if (biosFileInfo.size() != 128 * 1024) {
		*error = QObject::tr("Invalid BIOS size: %1 should be %2 bytes large")
				.arg(biosFileInfo.size())
				.arg(128 * 1024);
		return false;
	}

	return true;
}

QString PicoMachine::init(const QString &diskPath)
{
	QString error;
	QString cartPath = diskPath;

	PicoOpt = 0x0f | 0x20 | 0xe00; // | use_940, cd_pcm, cd_cdda
	PicoAutoRgnOrder = 0x184; // US, EU, JP

	// make temp buffer for alt renderer
	PicoDraw2FB = (unsigned char *)malloc((8+320)*(8+240+8));
	PicoInit();

	picoFrame = QImage(320, 240, QImage::Format_RGB16);
	md_screen = picoFrame.bits();

	// make sure we are in correct mode
	PicoDrawSetColorFormat(1);
	picoScanLine(0);
	Pico.m.dirtyPal = 1;
	PicoOpt &= ~0x4100;
	PicoOpt |= 0x0100;
	modes = ((Pico.video.reg[12]&1)<<2) ^ 0xc;

	if (!diskPath.endsWith(".gen")) {
		mcd_state *data = new mcd_state;
		memset(data, 0, sizeof(mcd_state));
		Pico.rom = picoRom = (u8 *)data;
		// CD
		if (!Insert_CD(diskPath, &error))
			return error;
		PicoMCD |= 1;
		picoMcdOpt |= PicoMcdEnabled;

		if (!findMcdBios(&cartPath, &error))
			return error;
	}

	if (!picoCart.open(cartPath, &error))
		return error;

	// pal/ntsc might have changed, reset related stuff
	int targetFps = Pico.m.pal ? 50 : 60;
	setFrameRate(targetFps);

	PicoReset(1);
	PicoOpt &= ~0x10000;

	return QString();
}

void PicoMachine::shutdown()
{
	picoFrame = QImage();
}

void PicoMachine::reset()
{
	PicoReset(0);
}

void PicoMachine::emulateFrame(bool drawEnabled)
{
	updateInput();

	md_screen = picoFrame.bits();
	int oldmodes = modes;

	// check for mode changes
	modes = ((Pico.video.reg[12]&1)<<2)|(Pico.video.reg[1]&8);
	if (modes != oldmodes) {
		int w = ((modes & 4) ? 320 : 256);
		int h = ((modes & 8) ? 240 : 224);
		setVideoSrcRect(QRectF(0, 0, w, h));
	}
	PicoSkipFrame = (drawEnabled ? 0 : 2);
	PicoFrame();
}

const QImage &PicoMachine::frame() const
{
	return picoFrame;
}

int main(int argc, char *argv[])
{
	if (argc < 2)
		return -1;
	QApplication app(argc, argv);
	MachineView view(&picoMachine, argv[1]);
	return app.exec();
}
