#include "machine.h"
#include "snes9x.h"
#include "memmap.h"
#include "apu.h"
#include "gfx.h"
#include "soundux.h"
#include "machineview.h"
#include "srtc.h"
#include "dma.h"
#include "sdd1.h"
#include <QApplication>

SnesCpu cpu;
SnesPpu ppu;
SnesMemory memory;
SnesApu apu;
SnesSound sound;
SnesDma dma;

SnesMachine::SnesMachine(QObject *parent) :
	IMachine("snes", parent),
	m_frame(256, 239, QImage::Format_RGB16) {
	setFrameRate(60);
	setVideoSrcRect(QRectF(0.0f, 0.0f, 256.0f, 239.f));
}

static bool romLoaded = false;
static int soundSampleCount;
static bool initialized = false;
static volatile bool rendered = false;
static int gamepad = 0;

void setDefaultSettings() {
	// try to start emu
	ZeroMemory (&Settings, sizeof (Settings));

	Settings.SoundBufferSize = 0;
	Settings.JoystickEnabled = FALSE;
	Settings.CyclesPercentage = 100;
	Settings.DisableSoundEcho = FALSE;
	Settings.APUEnabled = 1;// TODO (iSettings.iEnableSpeedHack ? 3 : 1);
	Settings.H_Max = SNES_CYCLES_PER_SCANLINE;
	Settings.SkipFrames = AUTO_FRAMERATE;
	Settings.AutoMaxSkipFrames = 10;
	Settings.Shutdown = Settings.ShutdownMaster = TRUE;
	Settings.FrameTimePAL = 20000;
	Settings.FrameTimeNTSC = 16667;
	Settings.FrameTime = Settings.FrameTimeNTSC;
	Settings.DisableSampleCaching = FALSE;
	Settings.DisableMasterVolume = FALSE;
	Settings.Mouse = FALSE;
	Settings.SuperScope = FALSE;
	Settings.MultiPlayer5 = FALSE;
	Settings.ControllerOption = 0;
	Settings.ForceTransparency = TRUE;
	Settings.ForceNoTransparency = FALSE;
	Settings.Transparency = TRUE;
	Settings.SixteenBit = TRUE;
	Settings.SupportHiRes = FALSE;
	Settings.NetPlay = FALSE;
	Settings.ServerName[0] = 0;
	Settings.AutoSaveDelay = 30;
	Settings.ApplyCheats = FALSE;
	Settings.TurboMode = FALSE;
	Settings.TurboSkipFrames = 15;
	Settings.HBlankStart = (256 * Settings.H_Max) / SNES_HCOUNTER_MAX;
	Settings.SoundSync = FALSE;
	Settings.SixteenBitSound = TRUE;
	Settings.SoundPlaybackRate = 44100;//22050;
	Settings.os9x_hack = FALSE;
	Settings.SoundSync = FALSE;
	Settings.SixteenBitSound = true;
	Settings.Stereo = TRUE;
	soundSampleCount = Settings.SoundPlaybackRate / (Settings.PAL ? 50 : 60);
	if (Settings.Stereo)
		soundSampleCount = soundSampleCount * 2;
	so.stereo = Settings.Stereo;
	so.playback_rate = Settings.SoundPlaybackRate;
}

extern "C" void S9xSyncSpeed(void) {
	IPPU.RenderThisFrame = TRUE;
}

extern "C" bool8_32 S9xDeinitUpdate(int width, int height, bool8_32 a) {
	rendered = true;
	return true;
}

extern "C" u32 S9xReadJoypad(int which1) {
	if (which1)
		return 0;
	return gamepad;
}

extern "C" void S9xMessage(int type, int number, const char *message)
{ qDebug("%d %d %s", type, number, message); }

extern "C" void S9xLoadSDD1Data(void) {
	Settings.SDD1Pack = TRUE;
	Memory.FreeSDD1Data();

	//TODO: now what?
	Settings.SDD1Pack = FALSE;
}


extern "C" const char *S9xGetFilename(const char *ex) // ex is like ".srm"
{
	static char filename[PATH_MAX];
	char *p;

	strcpy(filename, Memory.ROMFilename);
	for (p = filename + strlen(filename) - 1; p > filename + 3; p--)
		if (*p == '.' || *p == '\\')
			break;
	strcpy(p, ex);

	return filename;
}

extern "C" bool8 S9xReadMousePosition(int /* which1 */, int &/* x */,
		int & /* y */, u32 & /* buttons */)
{
	return (FALSE);
}

extern "C" bool8 S9xReadSuperScopePosition(int & /* x */, int & /* y */,
		u32 & /* buttons */)
{
	return (FALSE);
}

extern "C" const char *S9xGetFilenameInc(const char *e)
{
	return e;
}

extern "C" const char *S9xBasename(const char *f)
{
	const char *p;
	if ((p = strrchr(f, '/')) != NULL || (p = strrchr(f, '\\')) != NULL)
		return (p + 1);
	return (f);
}

void _splitpath(const char *path, char *drive, char *dir, char *fname,
		char *ext)
{
	//TODO!


	*drive = 0;

	char *slash = (char*) strrchr(path, '/');
	if (!slash)
		slash = (char*) strrchr(path, '\\');

	char *dot = (char*) strrchr(path, '.');

	if (dot && slash && dot < slash)
		dot = NULL;

	if (!slash)
	{
		strcpy(dir, "");
		strcpy(fname, path);
		if (dot)
		{
			*(fname + (dot - path)) = 0;
			strcpy(ext, dot + 1);
		}
		else
			strcpy(ext, "");
	}
	else
	{
		strcpy(dir, path);
		*(dir + (slash - path)) = 0;
		strcpy(fname, slash + 1);
		if (dot)
		{
			*(fname + (dot - slash) - 1) = 0;
			strcpy(ext, dot + 1);
		}
		else
			strcpy(ext, "");
	}
}

void _makepath(char *path, const char *, const char *dir, const char *fname,
		const char *ext)
{
	if (dir && *dir)
	{
		strcpy(path, dir);
		strcat(path, "/");
	}
	else
		*path = 0;
	strcat(path, fname);
	if (ext && *ext)
	{
		strcat(path, ".");
		strcat(path, ext);
	}
}

QString SnesMachine::setDisk(const QString &path) {
	setDefaultSettings();
	S9xSetPlaybackRate(so.playback_rate);
	if (!initialized) {
		GFX.Pitch = 256 * 2;
		GFX.RealPitch = 256 * 2;
		GFX.PPL = GFX.Pitch >> 1;
		GFX.PPLx2 = GFX.Pitch;
		GFX.ZPitch = GFX.Pitch >> 1;
		GFX.Screen = (u8 *)m_frame.bits();
		GFX.SubScreen = (u8 *) malloc(GFX.RealPitch * 239);
		GFX.ZBuffer = (u8 *) malloc(GFX.PPL * 239);
		GFX.SubZBuffer = (u8 *) malloc(GFX.PPL * 239);
		GFX.Delta = (GFX.SubScreen - GFX.Screen) >> 1;

		if (!GFX.Screen || !GFX.SubScreen || !GFX.ZBuffer || !Memory.Init() || !S9xInitAPU() || !GFX.SubZBuffer )
			return QString("SNES emulation init failed!");
		S9xInitSound();
		initialized = true;
		if (!S9xGraphicsInit())
			return QString("SNES emulation init failed!");
	}
	S9xReset();
	setDefaultSettings();
	S9xSetPlaybackRate(so.playback_rate);
	S9xSetSoundMute(FALSE);

	bool success = Memory.LoadROM((path + ".smc").toAscii().constData());
	qDebug(qPrintable(path));
	if (!success) {
		qDebug("load rom failed");
		exit(-1);
	}
	Settings.FrameTime = Settings.PAL ? Settings.FrameTimePAL : Settings.FrameTimeNTSC;
	Memory.ROMFramesPerSecond = Settings.PAL ? 50 : 60;
	setFrameRate(Memory.ROMFramesPerSecond);

	romLoaded = true;
	S9xMainLoop();
	return QString();
}

void SnesMachine::emulateFrame(bool drawEnabled) {
	if (drawEnabled)
		GFX.Screen = (u8 *)m_frame.bits();
	rendered = false;
	while (!rendered)
		S9xMainLoop();
}

const QImage &SnesMachine::frame() const
{ return m_frame; }

int SnesMachine::fillAudioBuffer(char *stream, int streamSize) {
	int count = qMin(streamSize/2, soundSampleCount);
	S9xMixSamples((short int *)stream, count);
	return count * 2;
}

static void setPadBit(int mask, bool on) {
	if (on)
		gamepad |=  mask;
	else
		gamepad &= ~mask;
}

void SnesMachine::setPadKey(PadKey key, bool state) {
	switch (key) {
	case Left_PadKey:	setPadBit(SNES_LEFT_MASK, state); break;
	case Right_PadKey:	setPadBit(SNES_RIGHT_MASK, state); break;
	case Up_PadKey:		setPadBit(SNES_UP_MASK, state); break;
	case Down_PadKey:	setPadBit(SNES_DOWN_MASK, state); break;
	case A_PadKey:		setPadBit(SNES_A_MASK, state); break;
	case B_PadKey:		setPadBit(SNES_B_MASK, state); break;
	case X_PadKey:		setPadBit(SNES_X_MASK, state); break;
	case Y_PadKey:		setPadBit(SNES_Y_MASK, state); break;
	case L_PadKey:		setPadBit(SNES_TL_MASK, state); break;
	case R_PadKey:		setPadBit(SNES_TR_MASK, state); break;
	case Start_PadKey:	setPadBit(SNES_START_MASK, state); break;
	case Select_PadKey:	setPadBit(SNES_SELECT_MASK, state); break;
	case AllKeys:		setPadBit(0xFFFF, false); break;
	}
}

#define STATE_SERIALIZE_BUILDER(sl) \
STATE_SERIALIZE_BEGIN_##sl(SnesMachine, 1) \
	if (STATE_SERIALIZE_TEST_TYPE_##sl) { \
		S9xSRTCPreSaveState(); \
	} else { \
		S9xReset(); \
	} \
	STATE_SERIALIZE_SUBCALL_##sl(cpu) \
	STATE_SERIALIZE_SUBCALL_##sl(memory) \
	STATE_SERIALIZE_SUBCALL_##sl(ppu) \
	STATE_SERIALIZE_SUBCALL_##sl(dma) \
	STATE_SERIALIZE_SUBCALL_##sl(sound) \
	STATE_SERIALIZE_SUBCALL_##sl(apu) \
	if (!STATE_SERIALIZE_TEST_TYPE_##sl) { \
		S9xSetPCBase(ICPU.ShiftedPB + Registers.PC); \
		S9xReschedule(); \
		S9xSRTCPostLoadState(); \
		if (Settings.SDD1) \
			S9xSDD1PostLoadState(); \
		S9xSetSoundMute(FALSE); \
	} \
STATE_SERIALIZE_END_##sl(SnesMachine)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)

int main(int argc, char *argv[]) {
	if (argc < 2)
		return -1;
	QApplication app(argc, argv);
	MachineView view(new SnesMachine(), argv[1]);
	return app.exec();
}

void SnesMachine::reset()
{ S9xReset(); }
