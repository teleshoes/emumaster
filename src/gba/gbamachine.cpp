#include "gbamachine.h"
#include "gbapad.h"
#include <QFile>
#include <QtPlugin>

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

GBAMachine::GBAMachine(QObject *parent) :
	IMachine("gba", parent),
	m_frame(240, 160, QImage::Format_RGB16) {
	setFrameRate(60);
	m_pad = new GBAPad(this);

	screen_pixels_ptr = (quint16 *)m_frame.bits();
	loadBios();
}

// TODO remove
extern "C" void qDebugC(const char *s, int a, int b) {
	qDebug(s, a, b);
}

void GBAMachine::loadBios() {
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

bool GBAMachine::save(QDataStream &s) {
	// TODO implement
	return true;
}

bool GBAMachine::load(QDataStream &s) {
	// TODO implement
	return true;
}

QString GBAMachine::setDisk(const QString &path) {
	if (!m_biosError.isEmpty())
		return m_biosError;
	// TODO call once
	init_gamepak_buffer();
	if (load_gamepak((path+".gba").toAscii().constData()) == -1)
		return "Could not load ROM";
	reset_machine();
	skip_next_frame = 0;
	m_cycles = init_machine();
	return QString();
}

quint32 GBAMachine::diskCrc() const {
	// TODO implement
	return 1;
}

const QImage & GBAMachine::frame() const
{ return m_frame; }

QRectF GBAMachine::videoSrcRect() const
{ return QRectF(0.0f, 0.0f, 240.0f, 160.f); }

// TODO scale preserve aspect alike function
QRectF GBAMachine::videoDstRect() const
{ return QRectF(67.0f, 0.0f, 240.0f*3.0f, 160.0f*3.0f); }

void GBAMachine::updateSettings() {
	sound_frequency = audioSampleRate();
	global_enable_audio = isAudioEnabled();
}

const char *GBAMachine::grabAudioBuffer(int *size) {
	*size = sound_callback(m_soundBuffer, 16384);
	return m_soundBuffer;
}

void GBAMachine::emulateFrame(bool drawEnabled) {
	skip_next_frame = drawEnabled;
	screen_pixels_ptr = (quint16 *)m_frame.bits();
	m_cycles = execute_arm(m_cycles);
}

void GBAMachine::setPadKey(PadKey key, bool state) {
	switch (key) {
	case Left_PadKey:	m_pad->setKey(GBAPad::Left_PadKey, state); break;
	case Right_PadKey:	m_pad->setKey(GBAPad::Right_PadKey, state); break;
	case Up_PadKey:		m_pad->setKey(GBAPad::Up_PadKey, state); break;
	case Down_PadKey:	m_pad->setKey(GBAPad::Down_PadKey, state); break;
	case A_PadKey:		m_pad->setKey(GBAPad::A_PadKey, state); break;
	case B_PadKey:		m_pad->setKey(GBAPad::B_PadKey, state); break;
	case X_PadKey:		m_pad->setKey(GBAPad::R_PadKey, state); break;
	case Y_PadKey:		m_pad->setKey(GBAPad::L_PadKey, state); break;
	case Start_PadKey:	m_pad->setKey(GBAPad::Start_PadKey, state); break;
	case Select_PadKey:	m_pad->setKey(GBAPad::Select_PadKey, state); break;
	case AllKeys:		m_pad->setKey(GBAPad::All_PadKeys, state); break;
	default: break;
	}
}

Q_EXPORT_PLUGIN2(gba, GBAMachine)

void GBAMachine::invalidateFrame() {
	screen_pixels_ptr = (quint16 *)m_frame.bits();
}
