#include "machineview.h"
#include "imachine.h"
#include "machinethread.h"
#include "hostvideo.h"
#include "hostaudio.h"
#include "hostinput.h"
#include "settingsview.h"
#include "machineimageprovider.h"
#include "machinestatelistmodel.h"
#include "gamegeniecodelistmodel.h"
#include <misc/gamegeniecode.h>
#include <QDeclarativeView>
#include <QDeclarativeContext>
#include <QCloseEvent>
#include <QApplication>
#include <QDeclarativeEngine>
#include <QTimer>
#include <QDir>

MachineView::MachineView(IMachine *machine, const QString &diskName, QWidget *parent) :
	QObject(parent),
	m_machine(machine),
	m_running(false),
	m_backgroundCounter(qAbs(qrand())/2),
	m_diskName(diskName),
	m_wantClose(false),
	m_autoLoadOnStart(true),
	m_autoSaveOnExit(true) {
	Q_ASSERT(m_machine != 0);

	m_thread = new MachineThread(this);

	m_hostInput = new HostInput(this);
	m_hostAudio = new HostAudio(this);
	m_hostVideo = new HostVideo(this);

	m_hostVideo->installEventFilter(m_hostInput);
	m_hostVideo->m_srcRect = m_machine->videoSrcRect();
	m_hostVideo->m_dstRect = m_machine->videoDstRect();

	QString error = m_machine->setDisk(QString("%1/%2/%3")
									   .arg(romDirPath())
									   .arg(m_machine->name())
									   .arg(diskName));

	m_stateListModel = new MachineStateListModel(this);
	m_settingsView = new SettingsView(this);
	m_settingsView->engine()->addImageProvider("machine", new MachineImageProvider(this, m_stateListModel));
	m_settingsView->rootContext()->setContextProperty("backgroundPath", "");
	m_settingsView->rootContext()->setContextProperty("settingsView", static_cast<QObject *>(m_settingsView));
	m_settingsView->rootContext()->setContextProperty("machineView", static_cast<QObject *>(this));
	m_settingsView->rootContext()->setContextProperty("video", static_cast<QObject *>(m_hostVideo));
	m_settingsView->rootContext()->setContextProperty("audio", static_cast<QObject *>(m_hostAudio));
	m_settingsView->rootContext()->setContextProperty("input", static_cast<QObject *>(m_hostInput));
	m_settingsView->rootContext()->setContextProperty("machine", static_cast<QObject *>(m_machine));
	m_settingsView->rootContext()->setContextProperty("stateListModel", static_cast<QObject *>(m_stateListModel));
	QObject::connect(m_settingsView->engine(), SIGNAL(quit()), SLOT(close()));

	if (!error.isEmpty())
		showError(error);

	m_gameGenieCodeListModel = new GameGenieCodeListModel(this);
	m_gameGenieCodeListModel->load();
	m_settingsView->rootContext()->setContextProperty("gameGenieCodeListModel", static_cast<QObject *>(m_gameGenieCodeListModel));

	m_machine->moveToThread(m_thread);
	m_machine->updateSettings();

	m_hostVideo->setAttribute(Qt::WA_QuitOnClose, false);
	m_settingsView->setAttribute(Qt::WA_QuitOnClose, false);

	if (error.isEmpty()) {
		m_machine->emulateFrame(false);
		if (m_autoLoadOnStart)
			m_stateListModel->loadState(-2);
	}

	QTimer::singleShot(100, this, SLOT(resume()));
}

MachineView::~MachineView() {
	if (m_hostVideo->m_error.isEmpty()) {
		if (m_thread->isRunning())
			m_thread->wait();
		if (m_autoSaveOnExit)
			m_stateListModel->saveState(-2);

		// auto save screenshot
		if (!QFile::exists(screenShotPath()))
			saveScreenShot();

		m_gameGenieCodeListModel->save();
	}
	delete m_machine;
	delete m_hostVideo;
	delete m_settingsView;
	if (!parent())
		qApp->exit();
}

void MachineView::showError(const QString &text) {
	Q_ASSERT(!text.isEmpty());
	m_hostVideo->m_error = text;
	m_hostVideo->setMyVisible(true);
	m_settingsView->setMyVisible(false);
}

// two-stage pause preventing deadlocks
void MachineView::pause() {
	if (!m_running)
		return;
	QObject::disconnect(m_thread, SIGNAL(frameGenerated()),
						m_hostVideo, SLOT(repaint()));
	m_thread->pause();
	QMetaObject::invokeMethod(this, "pauseStage2", Qt::QueuedConnection);
}

void MachineView::pauseStage2() {
	if (m_thread->isRunning())
		QTimer::singleShot(10, this, SLOT(pauseStage2()));
	m_thread->wait();
	m_running = false;
	if (!m_wantClose) {
		QString path = QString("image://machine/screenShotGrayscaled%1").arg(m_backgroundCounter++);
		m_settingsView->rootContext()->setContextProperty("backgroundPath", path);
		if (m_settingsView->source().isEmpty())
			m_settingsView->setSource(QUrl::fromLocalFile(QString("../qml/%1/main.qml").arg(m_machine->name())));
		m_settingsView->setMyVisible(true);
		m_hostVideo->setMyVisible(false);
	} else {
		close();
	}
}

void MachineView::resume() {
	if (m_running)
		return;
	m_machine->m_audioEnable = m_hostAudio->isEnabled();
	m_machine->m_audioSampleRate = m_hostAudio->sampleRate();
	m_machine->m_audioStereoEnable = m_hostAudio->isStereoEnabled();
	m_machine->updateSettings();

	m_hostVideo->setMyVisible(true);
	m_settingsView->setMyVisible(false);

	QObject::connect(m_thread, SIGNAL(frameGenerated()),
					 m_hostVideo, SLOT(repaint()),
					 Qt::BlockingQueuedConnection);
	if (m_hostVideo->m_error.isEmpty()) {
		m_machine->setGameGenieCodeList(m_gameGenieCodeListModel->enabledList());
		m_thread->resume();
		m_hostVideo->setupSwipe(m_hostInput->isSwipeEnabled());
	} else {
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
			.save(screenShotPath());
}

QString MachineView::romDirPath()
{ return QString("%1/MyDocs/emumaster").arg(getenv("HOME")); }

QString MachineView::userDataDirPath()
{ return QString("%1/.emumaster").arg(getenv("HOME")); }

void MachineView::buildLocalDirTree() {
	QDir dir(getenv("HOME"));
	dir.mkdir(".emumaster");
	dir.cd(".emumaster");
	dir.mkdir("save");
	dir.mkdir("icon");
	dir.mkdir("screenshot");
	dir.mkdir("cheat");
	dir = QDir(getenv("HOME"));
	dir.cd("MyDocs");
	dir.mkdir("emumaster");
	dir.cd("emumaster");
	dir.mkdir("nes");
	// TODO add directory for other consoles
}

bool MachineView::isGameGenieCodeValid(const QString &s) {
	GameGenieCode ggc;
	return ggc.parse(s);
}

QString MachineView::screenShotPath() const {
	return QString("%1/screenshot/%2%3.jpg")
			.arg(userDataDirPath())
			.arg(m_machine->name())
			.arg(m_diskName);
}
