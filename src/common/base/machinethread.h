#ifndef MACHINETHREAD_H
#define MACHINETHREAD_H

class IMachine;
class HostAudio;
#include <QThread>

class MachineThread : public QThread {
    Q_OBJECT
public:
	explicit MachineThread(IMachine *machine);
	~MachineThread();

	int frameSkip() const;
	void setFrameSkip(int n);
public slots:
	void resume();
	void pause();
signals:
	void frameGenerated(bool videoOn);
protected:
	void run();
private:
	IMachine *m_machine;
	volatile bool m_running;
	volatile bool m_inFrameGenerated;

	int m_frameSkip;

	friend class HostVideo;
};

inline int MachineThread::frameSkip() const
{ return m_frameSkip; }

#endif // MACHINETHREAD_H
