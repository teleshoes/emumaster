#ifndef MACHINETHREAD_H
#define MACHINETHREAD_H

class MachineView;
class IMachine;
class HostAudio;
#include <QThread>

class MachineThread : public QThread {
    Q_OBJECT
public:
	explicit MachineThread(MachineView *parent);
	~MachineThread();

	void setAudioEnabled(bool on);
	void updateSettings();
public slots:
	void resume();
	void pause();
signals:
	void frameGenerated();
protected:
	void run();
private:
	void sendAudioFrame(HostAudio *hostAudio, IMachine *machine);

	volatile bool m_running;
	volatile bool m_inFrameGenerated;

	friend class HostVideo;
};

#endif // MACHINETHREAD_H
