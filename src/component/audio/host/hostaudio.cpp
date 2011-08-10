#include "hostaudio.h"

HostAudio::HostAudio(QObject *parent) :
	QObject(parent),
	m_audio(0),
	m_io(0) {
}

void HostAudio::setFormat(const QAudioFormat &fmt) {
	delete m_audio;
	m_audio = new QAudioOutput(fmt, this);
	m_io = m_audio->start();
}

void HostAudio::write(const char *data, int size) {
	Q_ASSERT_X(m_io != 0, "HostAudio", "set format first");
	m_io->write(data, size);
}

QList<int> HostAudio::supportedSampleRates() const
{ return QAudioDeviceInfo::defaultOutputDevice().supportedSampleRates(); }
