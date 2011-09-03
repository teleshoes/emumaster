#include "hostaudio.h"
#include "imachine.h"

// TODO async pulseaudio

HostAudio::HostAudio(IMachine *machine) :
	m_io(0),
	m_machine(machine) {
}

HostAudio::~HostAudio() {
	close();
}

void HostAudio::open(int sampleRate) {
	pa_sample_spec fmt;
	fmt.channels = 2;
	fmt.format = PA_SAMPLE_S16LE;
	fmt.rate = sampleRate;

	m_io = pa_simple_new(0, "emumaster", PA_STREAM_PLAYBACK, 0, "audio", &fmt, 0, 0, 0);
}

void HostAudio::close() {
	if (m_io) {
		pa_simple_free(m_io);
		m_io = 0;
	}
}

void HostAudio::sendFrame() {
	int size = m_machine->fillAudioBuffer(buffer, sizeof(buffer));
	if (!m_io)
		return;
	int error;
	pa_simple_write(m_io, buffer, size, &error);
}
