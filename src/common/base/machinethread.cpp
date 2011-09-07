#include "machinethread.h"
#include "imachine.h"
#include "hostaudio.h"
#include <QMutex>
#include <QWaitCondition>
#include <QDateTime>

MachineThread::MachineThread(IMachine *machine) :
	m_machine(machine),
	m_running(false),
	m_inFrameGenerated(false),
	m_frameSkip(1) {
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

void MachineThread::run() {
	qreal frameTime = 1000.0 / m_machine->frameRate();
	QTime time;
	time.start();
	qreal currentFrameTime = 0;
	int frameCounter = 0;
	while (m_running) {
		qreal currentTime = time.elapsed();
		currentFrameTime += frameTime;
		if (currentTime < currentFrameTime && frameCounter == 0) {
			m_machine->emulateFrame(true);
			m_inFrameGenerated = true;
			emit frameGenerated(true);
			m_inFrameGenerated = false;
			qreal currentTime = time.elapsed();
			if (currentTime < currentFrameTime)
				sleepMs(currentFrameTime - currentTime);
		} else {
			m_machine->emulateFrame(false);
			emit frameGenerated(false);
			if (frameCounter != 0) {
				qreal currentTime = time.elapsed();
				if (currentTime < currentFrameTime)
					sleepMs(currentFrameTime - currentTime);
			} else {
				currentFrameTime = 0;
				time.restart();
			}
		}
		if (++frameCounter > m_frameSkip)
			frameCounter = 0;
	}
}

void MachineThread::setFrameSkip(int n)
{ m_frameSkip = n; }
