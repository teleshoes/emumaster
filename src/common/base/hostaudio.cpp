#include "hostaudio.h"
#include "imachine.h"

static void contextStreamCallback(pa_context *context, void *userdata) {
	 if (!context || !userdata)
		 return;
	 HostAudio *hostAudio = reinterpret_cast<HostAudio *>(userdata);
	 switch (pa_context_get_state(context)) {
	 case PA_CONTEXT_READY:
	 case PA_CONTEXT_TERMINATED:
	 case PA_CONTEXT_FAILED:
		 pa_threaded_mainloop_signal(hostAudio->mainloop(), 0);
		 break;

	 case PA_CONTEXT_UNCONNECTED:
	 case PA_CONTEXT_CONNECTING:
	 case PA_CONTEXT_AUTHORIZING:
	 case PA_CONTEXT_SETTING_NAME:
		 break;
	 }
}

HostAudio::HostAudio(IMachine *machine) :
	m_mainloop(0),
	m_context(0),
	m_api(0),
	m_stream(0),
	m_machine(machine) {
}

HostAudio::~HostAudio() {
	close();
}

void HostAudio::open(int sampleRate) {
	m_mainloop = pa_threaded_mainloop_new();
	if (!m_mainloop) {
		qDebug("Could not acquire PulseAudio main loop");
		return;
	}
	m_api = pa_threaded_mainloop_get_api(m_mainloop);
	m_context = pa_context_new(m_api, "emumaster");
	pa_context_set_state_callback(m_context, contextStreamCallback, this);

	if (!m_context) {
		qDebug("Could not acquire PulseAudio device context");
		return;
	}
	if (pa_context_connect(m_context, 0, PA_CONTEXT_NOFLAGS, 0) < 0) {
		int error = pa_context_errno(m_context);
		qDebug("Could not connect to PulseAudio server: %s", pa_strerror(error));
		return;
	}
	pa_threaded_mainloop_lock(m_mainloop);
	if (pa_threaded_mainloop_start(m_mainloop) < 0) {
		qDebug("Could not start mainloop");
		return;
	}

	waitForStreamReady();

	pa_sample_spec fmt;
	fmt.channels = 2;
	fmt.format = PA_SAMPLE_S16LE;
	fmt.rate = sampleRate;

	pa_buffer_attr buffer_attributes;
	buffer_attributes.tlength = pa_bytes_per_second(&fmt) / 5;
	buffer_attributes.maxlength = buffer_attributes.tlength * 3;
	buffer_attributes.minreq = buffer_attributes.tlength / 3;
	buffer_attributes.prebuf = buffer_attributes.tlength;

	m_stream = pa_stream_new(m_context, "emumaster", &fmt, 0);
	if (!m_stream) {
		int error = pa_context_errno(m_context);
		qDebug("Could not acquire new PulseAudio stream: %s", pa_strerror(error));
		return;
	}
	pa_stream_flags_t flags = (pa_stream_flags_t)(PA_STREAM_ADJUST_LATENCY | PA_STREAM_INTERPOLATE_TIMING | PA_STREAM_AUTO_TIMING_UPDATE);
//	pa_stream_flags_t flags = (pa_stream_flags_t) (PA_STREAM_INTERPOLATE_TIMING | PA_STREAM_AUTO_TIMING_UPDATE | PA_STREAM_EARLY_REQUESTS);
	if (pa_stream_connect_playback(m_stream, 0, &buffer_attributes, flags, 0, 0) < 0) {
		int error = pa_context_errno(m_context);
		qDebug("Could not connect for playback: %s", pa_strerror(error));
		return;
	}

	waitForStreamReady();

	pa_threaded_mainloop_unlock(m_mainloop);
}

void HostAudio::close() {
	if (m_mainloop)
		pa_threaded_mainloop_stop(m_mainloop);
	if (m_stream) {
		pa_stream_unref(m_stream);
		m_stream = 0;
	}
	if (m_context) {
		pa_context_disconnect(m_context);
		pa_context_unref(m_context);
		m_context = 0;
	}
	if (m_mainloop) {
		pa_threaded_mainloop_free(m_mainloop);
		m_mainloop = 0;
	}
}

void HostAudio::sendFrame() {
	if (!m_stream)
		return;
	pa_threaded_mainloop_lock(m_mainloop);
	size_t size = -1;
	void *data;
	pa_stream_begin_write(m_stream, &data, &size);
	size = qMin(size, pa_stream_writable_size(m_stream));
	if (size)
		size = m_machine->fillAudioBuffer(reinterpret_cast<char *>(data), size);
	if (size)
		pa_stream_write(m_stream, data, size, 0, 0, PA_SEEK_RELATIVE);
	else
		pa_stream_cancel_write(m_stream);
	pa_threaded_mainloop_unlock(m_mainloop);
	pa_threaded_mainloop_signal(m_mainloop, 0);
}

void HostAudio::waitForStreamReady() {
	pa_context_state_t context_state = pa_context_get_state(m_context);
	while (context_state != PA_CONTEXT_READY) {
		context_state = pa_context_get_state(m_context);
		if (!PA_CONTEXT_IS_GOOD(context_state)) {
			int error = pa_context_errno(m_context);
			qDebug("Context state is not good: %s", pa_strerror(error));
			return;
		} else if (context_state == PA_CONTEXT_READY) {
			break;
		} else {
			//qDebug("PulseAudio context state is %d", context_state);
		}
		pa_threaded_mainloop_wait(m_mainloop);
	}
}
