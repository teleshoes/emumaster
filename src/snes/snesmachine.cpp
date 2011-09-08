#include "snesmachine.h"
#include "snes9x.h"
#include "memmap.h"
#include "apu.h"
#include "gfx.h"
#include "soundux.h"
#include "snapshot.h"
#include "machineview.h"
#include <QApplication>

SnesMachine *emu = 0;

SnesMachine::SnesMachine(QObject *parent) :
	IMachine("snes", parent),
	m_frame(256, 239, QImage::Format_RGB16) {
	setFrameRate(60);
	setVideoSrcRect(QRectF(0.0f, 0.0f, 256.0f, 239.f));
	emu = this;
}

bool iRomLoaded = false;
int iSampleCount;
bool iInitialized = false;
volatile bool rendered = false;

void setDefaultSettings()
{
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
	iSampleCount = Settings.SoundPlaybackRate / (Settings.PAL ? 50 : 60);
	if (Settings.Stereo)
		iSampleCount = iSampleCount * 2;
	so.stereo = Settings.Stereo;
	so.playback_rate = Settings.SoundPlaybackRate;
}

extern "C" void S9xSyncSpeed(void)
{
	IPPU.FrameSkip = 0;
	IPPU.RenderThisFrame = TRUE;
	IPPU.SkippedFrames = 0;
}

extern "C" bool8 S9xDeinitUpdate(int width, int height, bool8 a)
{
	qMemCopy(emu->m_frame.scanLine(0), GFX.Screen, 256*239*2);
	rendered = true;
	return true;
}

extern "C" uint32 S9xReadJoypad(int which1) {
	if (which1)
		return 0;
	return 0;
}

extern "C" void S9xExtraUsage() {}
extern "C" void S9xParseArg(char **argv, int &index, int argc) {}
extern "C" void S9xMessage(int /* type */, int /* number */, const char *message) {}
extern "C" void S9xGenerateSound(void) {}
extern "C" void S9xSetPalette(void) {}
void S9xExit() {}

extern "C" bool8 S9xInitUpdate()
{
	return true;
}

extern "C" void S9xLoadSDD1Data(void)
{
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
		int & /* y */, uint32 & /* buttons */)
{
	return (FALSE);
}

extern "C" bool8 S9xReadSuperScopePosition(int & /* x */, int & /* y */,
		uint32 & /* buttons */)
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

int yo_rand(void)
{
	static int yo_rand_val = 0;
	return ++yo_rand_val;
}

void S9xAutoSaveSRAM(void) {
}

bool8 S9xOpenSoundDevice(int mode, bool8 stereo, int buffer_size)
{
	return TRUE;
}

///////////////////////
// save stuff

int (*statef_open)(const char *fname, const char *mode);
int (*statef_read)(void *p, int l);
int (*statef_write)(void *p, int l);
void (*statef_close)();
//TODO: ADD ZIP SUPPORT!
//static gzFile state_gzfile = 0;
static FILE *state_file = 0;

int state_gzip_open(const char *fname, const char *mode)
{/*
 state_gzfile = gzopen(fname, mode);
 if(state_gzfile && strchr(mode, 'w'))
 gzsetparams(state_gzfile, 9, Z_DEFAULT_STRATEGY);
 return (int) state_gzfile;*/
	return 0;
}

int state_gzip_read(void *p, int l)
{
	//return gzread(state_gzfile, p, l);
	return 0;
}

int state_gzip_write(void *p, int l)
{
	//  return gzwrite(state_gzfile, p, l);
	return 0;
}

void state_gzip_close()
{
	//   gzclose(state_gzfile);
}

int state_unc_open(const char *fname, const char *mode)
{
	state_file = fopen(fname, mode);
	return (int) state_file;
}

int state_unc_read(void *p, int l)
{
	return fread(p, 1, l, state_file);
}

int state_unc_write(void *p, int l)
{
	return fwrite(p, 1, l, state_file);
}

void state_unc_close()
{
	fclose(state_file);
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
	S9xSetSoundMute(FALSE);
	if (!iInitialized)
	{
		GFX.Pitch = 256 * 2;
		GFX.RealPitch = 256 * 2;
		GFX.PPL = GFX.Pitch >> 1;
		GFX.PPLx2 = GFX.Pitch;
		GFX.ZPitch = GFX.Pitch >> 1;
		GFX.Screen = (uint8 *) malloc(GFX.RealPitch * 239);
		GFX.SubScreen = (uint8 *) malloc(GFX.RealPitch * 239);
		GFX.ZBuffer = (uint8 *) malloc(GFX.PPL * 239);
		GFX.SubZBuffer = (uint8 *) malloc(GFX.PPL * 239);
		GFX.Delta = (GFX.SubScreen - GFX.Screen) >> 1;

		if (!GFX.Screen || !GFX.SubScreen || !GFX.ZBuffer || !Memory.Init() || !S9xInitAPU() || !GFX.SubZBuffer )
		{
			exit(-1);
		}
		S9xInitSound();
		iInitialized = true;
		if (!S9xGraphicsInit())
		{
			exit(-1);
		}
	}
	S9xReset();
	setDefaultSettings();
	S9xSetPlaybackRate(so.playback_rate);
	S9xSetSoundMute(FALSE);

	bool success = !Memory.LoadROM((path + ".smc").toAscii().constData());
	qDebug(qPrintable(path));
	if (!success) {
		qDebug("load rom failed");
		exit(-1);
	}
	Settings.FrameTime = Settings.PAL ? Settings.FrameTimePAL : Settings.FrameTimeNTSC;
	Memory.ROMFramesPerSecond = Settings.PAL ? 50 : 60;

	iRomLoaded = true;
	S9xMainLoop();
	return QString();
}

void SnesMachine::emulateFrame(bool drawEnabled) {
	rendered = false;
	while (!rendered)
		S9xMainLoop();
}

const QImage & SnesMachine::frame() const
{
	return m_frame;
}

int SnesMachine::fillAudioBuffer(char *stream, int streamSize) {
	int count = qMin(streamSize/2, iSampleCount);
	S9xMixSamples((short int *)stream, count);
	return count * 2;
}

void SnesMachine::setPadKey(IMachine::PadKey key, bool state)
{
}

bool SnesMachine::save(QDataStream &s)
{
	return false;
}

bool SnesMachine::load(QDataStream &s)
{
	return false;
}

int main(int argc, char *argv[]) {
	if (argc < 2)
		return -1;
	QApplication app(argc, argv);
	MachineView view(new SnesMachine(), argv[1]);
	return app.exec();
}
