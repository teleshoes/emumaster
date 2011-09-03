#ifndef HOSTAUDIO_H
#define HOSTAUDIO_H

class IMachine;
#include <QObject>
#include <pulse/simple.h>

class HostAudio : public QObject {
    Q_OBJECT
public:
	explicit HostAudio(IMachine *machine);
	~HostAudio();

	void open(int sampleRate);
	void close();

	void sendFrame();
private:
	pa_simple *m_io;
	char buffer[16384];

	IMachine *m_machine;
};

#endif // HOSTAUDIO_H
