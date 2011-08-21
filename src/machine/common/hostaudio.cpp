#include "hostaudio.h"

HostAudio::HostAudio(QObject *parent) :
	QObject(parent),
	m_enable(true),
	m_stereoEnable(true),
	m_sampleRate(22050) {
	m_io = 0;
#if !defined(Q_OS_LINUX)
	m_audio = 0;
#endif
}

HostAudio::~HostAudio() {
	close();
}

void HostAudio::open() {
#if defined(Q_OS_LINUX)
	pa_sample_spec fmt;
	fmt.channels = (m_stereoEnable ? 2 : 1);
	fmt.format = PA_SAMPLE_S16LE;
	fmt.rate = m_sampleRate;

	m_io = pa_simple_new(0, "emumaster", PA_STREAM_PLAYBACK, 0, "audio", &fmt, 0, 0, 0);
#else
	QAudioFormat fmt;
	fmt.setByteOrder(QAudioFormat::LittleEndian);
	fmt.setChannelCount(m_stereo ? 2 : 1);
	fmt.setSampleSize(16);
	fmt.setSampleRate(m_sampleRate);
	fmt.setCodec(setCodec("audio/pcm"));
	fmt.setSampleType(QAudioFormat::SignedInt);
	m_audio = new QAudioOutput(fmt, this);
	m_io = m_audio->start();
#endif
}

void HostAudio::close() {
#if defined(Q_OS_LINUX)
	if (m_io)
		pa_simple_free(m_io);
#else
	delete m_audio;
	m_audio = 0;
#endif
	m_io = 0;
}

void HostAudio::write(const char *data, int size) {
	Q_ASSERT_X(m_io != 0, "HostAudio", "open device first");
	if (!m_io)
		return;
#if defined(Q_OS_LINUX)
	int error;
	pa_simple_write(m_io, data, size, &error);
#else
	m_io->write(data, size);
#endif
}

void HostAudio::setEnabled(bool on) {
	if (m_enable != on) {
		m_enable = on;
		emit enableChanged();
	}
}

void HostAudio::setStereoEnabled(bool on) {
	if (m_stereoEnable != on) {
		m_stereoEnable = on;
		emit stereoEnableChanged();
	}
}

void HostAudio::setSampleRate(int rate) {
	if (m_sampleRate != rate) {
		m_sampleRate = rate;
		emit sampleRateChanged();
	}
}
