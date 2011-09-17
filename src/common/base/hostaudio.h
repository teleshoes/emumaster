#ifndef HOSTAUDIO_H
#define HOSTAUDIO_H

class IMachine;
#include <QObject>
#include <pulse/pulseaudio.h>

class HostAudio : public QObject {
    Q_OBJECT
public:
	explicit HostAudio(IMachine *machine);
	~HostAudio();

	void open();
	void close();

	void sendFrame();

	pa_threaded_mainloop *mainloop() const;
private:
	void waitForStreamReady();

	pa_threaded_mainloop *m_mainloop;
	pa_context *m_context;
	pa_mainloop_api *m_api;
	pa_stream *m_stream;

	IMachine *m_machine;
};

inline pa_threaded_mainloop *HostAudio::mainloop() const
{ return m_mainloop; }

#endif // HOSTAUDIO_H
