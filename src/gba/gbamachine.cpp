#include "gbamachine.h"
#include "gbapad.h"
#include "machineview.h"
#include <QFile>
#include <QApplication>
#include <QThread>

extern "C" quint32 global_enable_audio;
extern "C" quint32 sound_frequency;
extern "C" int sound_callback(char *stream, int length);
extern "C" quint32 skip_next_frame;
extern "C" quint32 execute_arm(quint32 cycles);
extern "C" void reset_machine();
extern "C" quint32 load_gamepak(const char *name);
extern "C" char bios_rom[];
extern "C" quint32 init_machine();
extern "C" quint16 *screen_pixels_ptr;
extern "C" void init_gamepak_buffer();

static GbaMachine *gbaMachine = 0;

static bool first = true;

extern "C" void synchronize_machine() {
	if (first)
		first = false;
	else
		gbaMachine->m_consSem.release();
	if (gbaMachine->m_quit) {
		gbaMachine->m_quit = false;
		QThread::currentThread()->terminate();
	}
	gbaMachine->m_prodSem.acquire();
}

GbaMachine::GbaMachine(QObject *parent) :
	IMachine("gba", parent),
	m_frame(240, 160, QImage::Format_RGB16) {
	setFrameRate(60);
	setVideoSrcRect(QRectF(0.0f, 0.0f, 240.0f, 160.f));
	m_pad = new GbaPad(this);

	screen_pixels_ptr = (quint16 *)m_frame.bits();
	loadBios();
	gbaMachine = this;
	m_quit = false;
}

GbaMachine::~GbaMachine() {
	m_quit = true;
	while (m_quit)
		emulateFrame(false);
}

void GbaMachine::loadBios() {
	// TODO bios filename
	QString path = "/home/user/MyDocs/emumaster/gba/gba_bios.bin";
	QFile biosFile(path);

	bool loaded = false;
	bool supported = false;
	if (biosFile.open(QIODevice::ReadOnly)) {
		if (biosFile.read(bios_rom, 0x4000) == 0x4000) {
			loaded = true;
			supported = (bios_rom[0] == 0x18);
		}
	}
	// TODO change name
	if (!loaded) {
		m_biosError =
				"Sorry, but emulator requires a Gameboy Advance BIOS\n"
				"image to run correctly. Make sure to get an        \n"
				"authentic one, it'll be exactly 16384 bytes large  \n"
				"and should have the following md5sum value:        \n"
				"                                                   \n"
				"a860e8c0b6d573d191e4ec7db1b1e4f6                   \n"
				"                                                   \n"
				"When you do get it name it gba_bios.bin and put it \n"
				"in the \"emumaster/gba\" directory.                ";

	} else if (!supported) {
		m_biosError = "You have an incorrect BIOS image.";
	}
}

bool GbaMachine::save(QDataStream &s) {
	// TODO implement
	return true;
}

bool GbaMachine::load(QDataStream &s) {
	// TODO implement
	return true;
}

QString GbaMachine::setDisk(const QString &path) {
	if (!m_biosError.isEmpty())
		return m_biosError;
	// TODO call once
	init_gamepak_buffer();
	if (load_gamepak((path+".gba").toAscii().constData()) == -1)
		return "Could not load ROM";
	reset_machine();
	skip_next_frame = 1;

	GbaThread *t = new GbaThread();
	t->setParent(this);
	t->start();
	return QString();
}

const QImage &GbaMachine::frame() const
{ return m_frame; }

void GbaMachine::emulateFrame(bool drawEnabled) {
	skip_next_frame = !drawEnabled;
	screen_pixels_ptr = (quint16 *)m_frame.bits();
	m_prodSem.release();
	m_consSem.acquire();
}

void GbaMachine::setPadKey(PadKey key, bool state) {
	switch (key) {
	case Left_PadKey:	m_pad->setKey(GbaPad::Left_PadKey, state); break;
	case Right_PadKey:	m_pad->setKey(GbaPad::Right_PadKey, state); break;
	case Up_PadKey:		m_pad->setKey(GbaPad::Up_PadKey, state); break;
	case Down_PadKey:	m_pad->setKey(GbaPad::Down_PadKey, state); break;
	case A_PadKey:		m_pad->setKey(GbaPad::A_PadKey, state); break;
	case B_PadKey:		m_pad->setKey(GbaPad::B_PadKey, state); break;
	case X_PadKey:		m_pad->setKey(GbaPad::R_PadKey, state); break;
	case Y_PadKey:		m_pad->setKey(GbaPad::L_PadKey, state); break;
	case Start_PadKey:	m_pad->setKey(GbaPad::Start_PadKey, state); break;
	case Select_PadKey:	m_pad->setKey(GbaPad::Select_PadKey, state); break;
	case AllKeys:		m_pad->setKey(GbaPad::All_PadKeys, state); break;
	default: break;
	}
}

int GbaMachine::fillAudioBuffer(char *stream, int streamSize)
// TODO fix sound callback
{ return sound_callback(stream, sound_frequency/60*2*2); }

void GbaMachine::setAudioEnabled(bool on)
{ global_enable_audio = on; }

void GbaMachine::setAudioSampleRate(int sampleRate)
{ sound_frequency = sampleRate; }

int main(int argc, char *argv[]) {
	if (argc < 2)
		return -1;
	QApplication app(argc, argv);
	MachineView view(new GbaMachine(), argv[1]);
	return app.exec();
}

void GbaThread::run() {
	uint cycles = init_machine();
	execute_arm(cycles);
}
