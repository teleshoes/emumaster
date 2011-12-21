#include "machine.h"
#include "machineview.h"
#include "pico.h"
#include "emu2.h"
#include "cart.h"
#include "maemo.h"
#include "mix.h"
#include <QImage>
#include <QApplication>

PicoMachine picoMachine;
QImage picoFrame;

PicoMachine::PicoMachine() :
	IMachine("pico") {
}

void emu_setDefaultConfig(void)
{
	PicoOpt = 0x0f | 0x20 | 0xe00; // | use_940, cd_pcm, cd_cdda
	PsndRate = 44100;
	PicoAutoRgnOrder = 0x184; // US, EU, JP
	PicoCDBuffers = 64;
}

static void updateSound(int len)
{
	if (PicoOpt&8) len<<=1;

	/* avoid writing audio when lagging behind to prevent audio lag */
//	TODO if (PicoSkipFrame != 2)
//		maemo_sound_write(PsndOut, len<<1);
}

static int modes = 0;

#define SCREEN_WIDTH 320

void *md_screen = 0;
static short __attribute__((aligned(4))) sndBuffer[2*44100/50];
unsigned char *PicoDraw2FB = NULL;  // temporary buffer for alt renderer

static void cd_leds(void)
{
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

static int EmuScan16(unsigned int num, void *sdata)
{
	//if (!(Pico.video.reg[1]&8)) num += 8;
	DrawLineDest = (unsigned short *) md_screen + SCREEN_WIDTH*(num+1);

	return 0;
}

static void vidResetMode(void)
{
	PicoDrawSetColorFormat(1);
	PicoScan = EmuScan16;
	PicoScan(0, 0);
	Pico.m.dirtyPal = 1;
}

static void update_volume()
{
	int vol = 100;//currentConfig.volume;

	// set the right mixer func
	if (!(PicoOpt&8)) return; // just use defaults for mono
	if (vol >= 5)
		PsndMix_32_to_16l = mix_32_to_16l_stereo;
	else {
		mix_32_to_16l_level = 5 - vol;
		PsndMix_32_to_16l = mix_32_to_16l_stereo_lvl;
	}
}

QString PicoMachine::init(const QString &diskPath) {
	emu_setDefaultConfig();
	// make temp buffer for alt renderer
	PicoDraw2FB = (unsigned char *)malloc((8+320)*(8+240+8));
	PicoInit();
	// TODO PicoMCDopenTray = emu_msg_tray_open;

	picoFrame = QImage(320, 240, QImage::Format_RGB16);
	md_screen = picoFrame.bits();

	// make sure we are in correct mode
	vidResetMode();
	PicoOpt &= ~0x4100;
	PicoOpt |= 0x0100;
	Pico.m.dirtyPal = 1;
	modes = ((Pico.video.reg[12]&1)<<2) ^ 0xc;

	// pal/ntsc might have changed, reset related stuff
	int target_fps = Pico.m.pal ? 50 : 60;
	setFrameRate(target_fps);

	// prepare sound stuff
	int snd_excess_add;
	PsndRerate(0);
	snd_excess_add = ((PsndRate - PsndLen*target_fps)<<16) / target_fps;
	printf("starting audio: %i len: %i (ex: %04x) stereo: %i, pal: %i\n",
		   PsndRate, PsndLen, snd_excess_add, (PicoOpt&8)>>3, Pico.m.pal);
	PicoWriteSound = updateSound;
	PsndOut = sndBuffer;

	// prepare CD buffer
	if (PicoMCD & 1)
		PicoCDBufferInit();

	QString error;
	if (!picoCart.open(diskPath, &error))
		return error;
	PicoReset(1);
	PicoOpt &= ~0x10000;
	return QString();
}

void PicoMachine::shutdown() {
	picoFrame = QImage();
}

void PicoMachine::reset() {
	PicoReset(0);
}

void PicoMachine::emulateFrame(bool drawEnabled) {
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

const QImage &PicoMachine::frame() const {
	return picoFrame;
}

int PicoMachine::fillAudioBuffer(char *stream, int streamSize) {
	return 0;
}

void PicoMachine::setAudioEnabled(bool on) {
}

int main(int argc, char *argv[]) {
	if (argc < 2)
		return -1;
	QApplication app(argc, argv);
	MachineView view(&picoMachine, argv[1]);
	return app.exec();
}
