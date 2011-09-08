#include <sys/stat.h>
#include <errno.h>
#include <dlfcn.h>
#include "emulator.h"
#include <QString>
#include <QImage>
#include <QPainter>
#include <QApplication>
#include <QTimer>

//#include "emumedia.h"
#include "../psxcommon.h"
#include "../cdrom.h"
#include "../plugins.h"
#include "../sio.h"
#include "../misc.h"
#include "../r3000a.h"
//#include "../video/externals.h"
#include "minimal.h"

#define CONFIG_DIR		"/home/user/MyDocs/emumaster"

#define SCREEN_W		emu_screen_w
#define SCREEN_H		emu_screen_h
#define SCREEN_PITCH	(emu_screen_pitch)

void ejectCD(const QString &fileName) {
	SetCdOpenCaseTime(time(0) + 2);
	SetIsoFile(fileName.toAscii().constData());
}

enum {
	EMUSTATE_PAUSED,
	EMUSTATE_RUNNING,
	EMUSTATE_LOADING,
	EMUSTATE_REQUEST_PAUSE
};

//static pthread_mutex_t emuStateMutex;
//static pthread_cond_t emuStateCond;
static int emuState = EMUSTATE_PAUSED;
static int savedState = 0;

static unsigned int keyStates;
static bool initialized;
static bool romLoaded = false;
static bool flipScreen;
static int refreshRate;

static unsigned int refreshTime;
static u64 lastFrameDrawnTime;

static struct {
	int key;
	int duration;
} trackballEvents[2];


unsigned char *screenBuffer = 0;
int current_enable_audio = 0;
int global_enable_audio = 0;
int global_enable_cdda_audio = 0;
int __saved = 0;
int psx4all_emulating = 1;
char cfgfile[256];
char savestate_filename[1024] = {'\0'};
int global_spu_irq = 0;
int global_enable_frameskip = 1;
int global_rcnt_fix = 0;
unsigned int global_bias_cycle = 3;
int global_cdrom_precise = 0;
int scaled_width = 320;
int scaled_height = 240;
int global_scale_stretch = 0;

static QImage frame;

extern int stop;
	
extern BOOL displayFrameInfo;
extern BOOL enableAbbeyHack;
extern u32 DrawingCount[4];
  
//extern EmuMedia *createEmuMedia();
extern "C" void setScreenSize(int width, int height);

extern "C" int iResX;
extern "C" int iResY;
extern "C" int updateAspect;
extern "C" long GPU_getScreenshot(u8 * pMem);

extern "C" void qDebugC(const char *msg) {
	qDebug(msg);
}

static void *emuThreadProc(void *arg)
{
        
  stop = 0;
  romLoaded = true;
  Config.SpuIrq = global_spu_irq;
  Config.RCntFix = global_rcnt_fix;
  Config.Cdda = (global_enable_cdda_audio ? 0 : 1);
  emuState = EMUSTATE_RUNNING;
  
  qDebug("A");

  if (SysInit() == -1) 
  {
    gp2x_deinit();
    emuState = EMUSTATE_PAUSED;
    return NULL;
  }

  ejectCD("/home/user/MyDocs/emumaster/psx/Kula World.iso");

  qDebug("B");

  if (LoadPlugins() == -1) 
  {
    gp2x_deinit();
    emuState = EMUSTATE_PAUSED;
    return NULL;
  }

  qDebug("C");

  CheckCdrom();

  SysReset();

  qDebug("D");

  if(psx4all_emulating == 0)
  {
    SysClose();
    gp2x_deinit();
    return NULL;
  }

  qDebug("E");

  if (LoadCdrom() == -1) 
  {
    SysClose();
    gp2x_deinit();
    emuState = EMUSTATE_PAUSED;
    return NULL;
  }

  qDebug("F");

  if(psx4all_emulating)
  {
    psxCpu->Execute();
  }

  qDebug("G");

  SysClose();
  gp2x_deinit();
	return NULL;
}

static void emu_config()
{
  //memset(&Config, 0, sizeof(PcsxConfig));
  Config.UseNet = 0;
  Config.HLE = 0;
  strcpy(cfgfile, CONFIG_DIR"/Pcsx.cfg");
  strcpy(Config.Net, _("Disabled"));

  strcpy(Config.Mcd1, CONFIG_DIR"/mcd001.mcr");
  strcpy(Config.Mcd2, CONFIG_DIR"/mcd002.mcr");
  Config.PsxAuto = 1;
  Config.Cdda = 1;
  Config.Xa = 0;
#ifdef DYNAREC
	Config.Cpu = CPU_DYNAREC;
#else
 	Config.Cpu = CPU_INTERPRETER;
#endif
	Config.Mdec = 0;
	Config.PsxOut = 0;
	Config.PsxType = 0;
	//Config.QKeys = 0;
	Config.RCntFix = 0;
	Config.Sio = 0;
	Config.SpuIrq = 0;
	Config.VSyncWA = 0;
	strcpy(Config.Bios, "scph1001.bin");
	strcpy(Config.BiosDir, "/home/user/MyDocs/emumaster/psx");
}

extern "C" u32 get_key_states()
{
	return 0;
}

extern "C" void setScreenSize(int width, int height)
{
	frame = QImage(width, height, QImage::Format_RGB16);
	screenBuffer = frame.scanLine(0);
}

static Thread *t = 0;

extern "C" void flip_screen()
{
	t->emitFrameGen();
	screenBuffer = frame.scanLine(0);
}

void init_emu()
{
	flipScreen = false;
	initialized = false;
	displayFrameInfo = true;

  emu_config();
  gp2x_init(1000, 16, 11025, 16, 1, 60, 1);

  initialized = true;
  emuThreadProc(NULL);
  qDebug("ended");
}

/*
extern "C" __attribute__((visibility("default")))
void Java_com_androidemu_Emulator_setNativeOption(JNIEnv *env, jobject self,
		jstring jname, jstring jvalue)
{
	const char *name = env->GetStringUTFChars(jname, NULL);
	const char *value = NULL;
	if (jvalue != NULL)
		value = env->GetStringUTFChars(jvalue, NULL);

	if (strcmp(name, "refreshRate") == 0) {
		if (strcmp(value, "default") == 0) 
		{
		  refreshRate = 60;
		}
		else
		{
		  refreshRate = atoi(value);
    }
	} else if (strcmp(name, "soundEnabled") == 0) {
		global_enable_audio = (strcmp(value, "true") == 0);

	} else if (strcmp(name, "cddaEnabled") == 0) {
		global_enable_cdda_audio = (strcmp(value, "true") == 0);

	} else if (strcmp(name, "flipScreen") == 0) {
		flipScreen = (strcmp(value, "true") == 0);

	} else if (strcmp(name, "spuIrq") == 0) {
		global_spu_irq = (strcmp(value, "true") == 0);

	} else if (strcmp(name, "rcntFix") == 0) {
		global_rcnt_fix = (strcmp(value, "true") == 0);

  } else if (strcmp(name, "showFPS") == 0) {
  	displayFrameInfo = (strcmp(value, "true") == 0);

  } else if (strcmp(name, "scaleMode") == 0) {
  	global_scale_stretch = (strcmp(value, "true") == 0);

  } else if (strcmp(name, "cycleMult2") == 0) {
  	global_bias_cycle = atoi(value);

  } else if (strcmp(name, "cdromPrecise") == 0) {
  	global_cdrom_precise = (strcmp(value, "true") == 0);
  		
  } else if (strcmp(name, "enableFrameskip") == 0) {
  	global_enable_frameskip = (strcmp(value, "true") == 0);		
    if(global_enable_frameskip)
    {
      DrawingCount[0] = 4;
      DrawingCount[1] = 4;
    }
    else
    {
      DrawingCount[0] = 0;
      DrawingCount[1] = 0;
    }
	}

	env->ReleaseStringUTFChars(jname, name);
	if (jvalue != NULL)
		env->ReleaseStringUTFChars(jvalue, value);
}*/

void reset_emulator() {
  SysReset();
  CheckCdrom();
  LoadCdrom();
}

void Widget::paintEvent(QPaintEvent *) {
	QPainter painter;
	painter.begin(this);
	painter.drawImage(0, 0, frame);
	painter.end();
}

void Thread::run() {
	frame = QImage(320, 240, QImage::Format_RGB16);
	screenBuffer = frame.scanLine(0);
	init_emu();
}

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);
	Widget *w = new Widget();
	w->showFullScreen();
	t = new Thread();
	QObject::connect(t, SIGNAL(frameGen()), w, SLOT(repaint()));
	QTimer::singleShot(10, t, SLOT(start()));
	return app.exec();
}

void Thread::emitFrameGen() {
	emit frameGen();
}
