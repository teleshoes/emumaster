#include "machineview.h"
#include "imachine.h"
#include "machinethread.h"
#include "hostvideo.h"
#include "hostaudio.h"
#include "hostinput.h"
#include "settingsview.h"
#include "machineimageprovider.h"
#include "machinestatelistmodel.h"
#include <QDeclarativeView>
#include <QDeclarativeContext>
#include <QCloseEvent>
#include <QApplication>
#include <QDeclarativeEngine>
#include <QTimer>
#include <QDir>
#include <QSettings>

MachineView::MachineView(IMachine *machine, const QString &diskName) :
	m_machine(machine),
	m_diskName(diskName),
	m_running(false),
	m_backgroundCounter(qAbs(qrand())/2),
	m_wantClose(false),
	m_pauseRequested(false),

	m_autoLoadOnStart(true),
	m_autoSaveOnExit(true),
	m_audioEnable(true),
	m_audioSampleRate(22050) {

	Q_ASSERT(m_machine != 0);

	m_thread = new MachineThread(m_machine);

	m_hostInput = new HostInput(m_machine);
	QObject::connect(m_hostInput, SIGNAL(pauseClicked()), SLOT(pause()));

	m_hostAudio = new HostAudio(m_machine);

	m_hostVideo = new HostVideo(m_machine, m_thread);
	m_hostVideo->installEventFilter(m_hostInput);
	QObject::connect(m_hostVideo, SIGNAL(wantClose()), SLOT(close()));
	QObject::connect(m_hostVideo, SIGNAL(minimized()), SLOT(pause()));

	loadSettings();

	QString error = m_machine->setDisk(QString("%1/%2")
									   .arg(m_machine->diskDirPath())
									   .arg(diskName));

	m_stateListModel = new MachineStateListModel(m_machine, diskName);

	m_settingsView = new SettingsView();
	QObject::connect(m_settingsView->engine(), SIGNAL(quit()), SLOT(close()));
	QObject::connect(m_settingsView, SIGNAL(wantClose()), SLOT(close()));

	m_settingsView->engine()->addImageProvider("machine", new MachineImageProvider(m_machine, m_stateListModel));
	m_settingsView->rootContext()->setContextProperty("backgroundPath", "");
	m_settingsView->rootContext()->setContextProperty("machineView", static_cast<QObject *>(this));
	m_settingsView->rootContext()->setContextProperty("machine", static_cast<QObject *>(m_machine));
	m_settingsView->rootContext()->setContextProperty("stateListModel", static_cast<QObject *>(m_stateListModel));

	if (!error.isEmpty())
		showError(error);

	m_machine->moveToThread(m_thread);

	if (error.isEmpty()) {
		m_machine->emulateFrame(false);
		if (m_autoLoadOnStart)
			m_stateListModel->loadState(-2);
	}
	QMetaObject::invokeMethod(this, "resume", Qt::QueuedConnection);
}

MachineView::~MachineView() {
	if (m_hostVideo->m_error.isEmpty()) {
		if (m_thread->isRunning())
			m_thread->wait();

		saveSettings();
		if (m_autoSaveOnExit)
			m_stateListModel->saveState(-2);

		// auto save screenshot
		if (!QFile::exists(m_machine->screenShotPath(m_diskName)))
			saveScreenShot();
	}
	delete m_thread;
	delete m_settingsView;
	delete m_stateListModel;
	delete m_hostVideo;
	delete m_hostAudio;
	delete m_hostInput;
	delete m_machine;
}

void MachineView::showError(const QString &text) {
	Q_ASSERT(!text.isEmpty());
	m_hostVideo->m_error = text;
	m_hostVideo->setMyVisible(true);
	m_settingsView->setMyVisible(false);
}

// two-stage pause preventing deadlocks
void MachineView::pause() {
	if (!m_running || m_pauseRequested)
		return;
	m_pauseRequested = true;
	QObject::disconnect(m_thread, SIGNAL(frameGenerated(bool)),
						this, SLOT(onFrameGenerated(bool)));
	m_thread->pause();
	QMetaObject::invokeMethod(this, "pauseStage2", Qt::QueuedConnection);
}

void MachineView::pauseStage2() {
	// the code below may be seen as bloat but it is needed
	// we are waiting for the thread to exit, but at the same
	// we allow blocking queued repaints from the thread
	if (m_thread->isRunning())
		QTimer::singleShot(10, this, SLOT(pauseStage2()));

	m_pauseRequested = false;
	m_running = false;
	if (!m_wantClose) {
		QString path = QString("image://machine/screenShotGrayscaled%1").arg(m_backgroundCounter++);
		m_settingsView->rootContext()->setContextProperty("backgroundPath", path);
		if (m_settingsView->source().isEmpty()) {
			m_settingsView->setSource(QUrl::fromLocalFile(QString("%1/qml/%2/main.qml")
														  .arg(IMachine::installationDirPath())
														  .arg(m_machine->name())));
		}
		if (m_audioEnable)
			m_hostAudio->close();
		m_settingsView->setMyVisible(true);
		m_hostVideo->setMyVisible(false);
	} else {
		close();
	}
}

void MachineView::resume() {
	if (m_running)
		return;
	m_hostVideo->setMyVisible(true);
	m_settingsView->setMyVisible(false);

	if (m_hostVideo->m_error.isEmpty()) {
		m_thread->resume();
	} else {
		QObject::connect(m_thread, SIGNAL(frameGenerated(bool)),
						 this, SLOT(onFrameGenerated(bool)),
						 Qt::BlockingQueuedConnection);
		if (m_audioEnable)
			m_hostAudio->open(m_audioSampleRate);
		m_hostVideo->repaint();
	}
	m_running = true;
}

bool MachineView::close() {
	m_wantClose = true;
	if (m_running) {
		pause();
		return false;
	} else {
		deleteLater();
		return true;
	}
}

void MachineView::saveScreenShot() {
	m_machine->frame().copy(m_machine->videoSrcRect().toRect())
			.save(m_machine->screenShotPath(m_diskName));
}

void MachineView::saveSettings() {
	QSettings s("elemental", "emumaster");
	s.setValue("swipeEnable", m_hostVideo->isSwipeEnabled());

	s.setValue("audioEnable", m_audioEnable);
	s.setValue("audioSampleRate", m_audioSampleRate);

	s.beginGroup(m_machine->name());
	s.setValue("frameSkip", m_thread->frameSkip());
	m_machine->saveSettings(s);
	s.endGroup();
}

void MachineView::loadSettings() {
	QSettings s("elemental", "emumaster");
	m_hostVideo->setSwipeEnabled(s.value("swipeEnable", false).toBool());

	m_audioEnable = s.value("audioEnable", true).toBool();
	m_audioSampleRate = s.value("audioSampleRate", 22050).toInt();

	s.beginGroup(m_machine->name());
	m_thread->setFrameSkip(s.value("frameSkip", 1).toInt());
	m_machine->loadSettings(s);
	s.endGroup();
}

void MachineView::onFrameGenerated(bool videoOn) {
	if (m_audioEnable)
		m_hostAudio->sendFrame();
	if (videoOn)
		m_hostVideo->repaint();
}

bool MachineView::isFpsVisible() const
{ return m_hostVideo->isFpsVisible(); }
int MachineView::frameSkip() const
{ return m_thread->frameSkip(); }
bool MachineView::isAudioEnabled() const
{ return m_audioEnable; }
int MachineView::audioSampleRate() const
{ return m_audioSampleRate; }
bool MachineView::isSwipeEnabled() const
{ return m_hostVideo->isSwipeEnabled(); }
bool MachineView::isPadVisible() const
{ return m_hostVideo->isPadVisible(); }

void MachineView::setFpsVisible(bool on) {
	if (m_hostVideo->isFpsVisible() != on) {
		m_hostVideo->setFpsVisible(on);
		emit fpsVisibleChanged();
	}
}

void MachineView::setFrameSkip(int n) {
	if (m_thread->frameSkip() != n) {
		m_thread->setFrameSkip(n);
		emit frameSkipChanged();
	}
}

void MachineView::setAudioEnabled(bool on) {
	if (m_audioEnable != on) {
		m_audioEnable = on;
		m_machine->setAudioEnabled(on);
		emit audioEnableChanged();
	}
}

void MachineView::setAudioSampleRate(int rate) {
	if (m_audioSampleRate != rate) {
		m_audioSampleRate = rate;
		m_machine->setAudioSampleRate(rate);
		emit audioSampleRateChanged();
	}
}

void MachineView::setSwipeEnabled(bool on) {
	if (m_hostVideo->isSwipeEnabled() != on) {
		m_hostVideo->setSwipeEnabled(on);
		emit swipeEnableChanged();
	}
}

void MachineView::setPadVisible(bool visible) {
	if (m_hostVideo->isPadVisible() != visible) {
		m_hostVideo->setPadVisible(visible);
		emit padVisibleChanged();
	}
}