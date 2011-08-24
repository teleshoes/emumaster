#include "machinethread.h"
#include "machineview.h"
#include "imachine.h"
#include "hostaudio.h"
#include "hostvideo.h"
#include <QMutex>
#include <QWaitCondition>
#include <QDateTime>

MachineThread::MachineThread(MachineView *parent) :
	QThread(parent),
	m_running(false),
	m_inFrameGenerated(false) {
}

MachineThread::~MachineThread() {
}

void MachineThread::resume() {
	m_running = true;
	start();
}

void MachineThread::pause() {
	m_running = false;
}

static void sleepMs(uint msecs) {
	QMutex mutex;
	mutex.lock();
	QWaitCondition waitCondition;
	waitCondition.wait(&mutex, msecs);
	mutex.unlock();
}

inline void MachineThread::sendAudioFrame(HostAudio *hostAudio, IMachine *machine) {
	if (hostAudio->isEnabled()) {
		int size;
		const char *data = machine->grabAudioBuffer(&size);
		hostAudio->write(data, size);
	}
}

void MachineThread::run() {
	MachineView *machineView = static_cast<MachineView *>(parent());
	IMachine *machine = machineView->m_machine;
	HostAudio *hostAudio = machineView->m_hostAudio;
	int frameSkip = machineView->m_hostVideo->frameSkip();
	qreal frameTime = 1000.0 / machine->frameRate();

	if (hostAudio->isEnabled())
		hostAudio->open();
	qreal currentFrameTime = QDateTime::currentMSecsSinceEpoch();
	int frameCounter = 0;
	while (m_running) {
		qreal currentTime = QDateTime::currentMSecsSinceEpoch();
		currentFrameTime += frameTime;
		if (currentTime < currentFrameTime && frameCounter == 0) {
			machine->emulateFrame(true);
			m_inFrameGenerated = true;
			emit frameGenerated();
			m_inFrameGenerated = false;
			sendAudioFrame(hostAudio, machine);
			currentTime = QDateTime::currentMSecsSinceEpoch();
			if (currentTime < currentFrameTime)
				sleepMs(currentFrameTime - currentTime);
		} else {
			machine->emulateFrame(false);
			sendAudioFrame(hostAudio, machine);
			if (frameCounter != 0) {
				currentTime = QDateTime::currentMSecsSinceEpoch();
				if (currentTime < currentFrameTime)
					sleepMs(currentFrameTime - currentTime);
			}
		}
		if (++frameCounter > frameSkip)
			frameCounter = 0;
	}
	if (hostAudio->isEnabled())
		hostAudio->close();
}
