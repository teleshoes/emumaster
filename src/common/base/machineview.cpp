/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "machineview.h"
#include "imachine.h"
#include "configuration.h"
#include "machinethread.h"
#include "hostvideo.h"
#include "hostaudio.h"
#include "hostinput.h"
#include "settingsview.h"
#include "stateimageprovider.h"
#include "statelistmodel.h"
#include "pathmanager.h"
#include <QDeclarativeView>
#include <QDeclarativeContext>
#include <QCloseEvent>
#include <QApplication>
#include <QDeclarativeEngine>
#include <QTimer>
#include <QDir>
#include <QSettings>
#include <QUdpSocket>

MachineView::MachineView(IMachine *machine, const QString &diskFileName) :
	m_machine(machine),
	m_diskFileName(diskFileName),
	m_running(false),
	m_backgroundCounter(qAbs(qrand())/2),
	m_quit(false),
	m_pauseRequested(false),
	m_audioEnable(true),
	m_autoSaveLoadEnable(true) {
	Q_ASSERT(m_machine != 0);

	Configuration::setupAppInfo();
	PathManager::instance()->setMachine(machine->name());

	m_thread = new MachineThread(m_machine);
	m_hostInput = new HostInput(m_machine);
	m_hostAudio = new HostAudio(m_machine);
	m_hostVideo = new HostVideo(m_hostInput, m_machine, m_thread);
	m_hostVideo->installEventFilter(m_hostInput);
	QObject::connect(m_hostVideo, SIGNAL(quit()), SLOT(close()));
	QObject::connect(m_hostVideo, SIGNAL(minimized()), SLOT(pause()));
	QObject::connect(m_hostInput, SIGNAL(quit()), SLOT(close()));
	QObject::connect(m_hostInput, SIGNAL(pause()), SLOT(pause()));
	QObject::connect(m_hostInput, SIGNAL(devicesChanged()),
					 SIGNAL(inputDevicesChanged()));

	m_stateListModel = new StateListModel(m_machine, m_diskFileName);
	m_thread->setStateListModel(m_stateListModel);

	if (!loadConfiguration())
		m_error = constructSlErrorString();

	if (m_error.isEmpty()) {
		QString diskPath = QString("%1/%2")
				.arg(PathManager::instance()->diskDirPath())
				.arg(m_diskFileName);
		m_error = m_machine->init(diskPath);
	}

	setupSettingsView();

	const char *method = "resume";
	if (m_error.isEmpty()) {
		QObject::connect(m_stateListModel, SIGNAL(slFailed()),
						 SLOT(onSlFailed()), Qt::QueuedConnection);
		#if defined(Q_WS_MAEMO_5)
			method = "pauseStage2";
		#endif
	} else {
		method = "pauseStage2";
	}
	QMetaObject::invokeMethod(this, method, Qt::QueuedConnection);
}

MachineView::~MachineView() {
	if (m_error.isEmpty()) {
		if (m_autoSaveLoadEnable)
			m_stateListModel->saveState(StateListModel::AutoSaveLoadSlot);
		// auto save screenshot
		saveScreenShotIfNotExists();
		m_machine->shutdown();
	}
	delete m_thread;
	delete m_settingsView;
	delete m_stateListModel;
	delete m_hostVideo;
	delete m_hostAudio;
	delete m_hostInput;
}

void MachineView::setupSettingsView() {
	m_settingsView = new SettingsView();
	QObject::connect(m_settingsView->engine(), SIGNAL(quit()), SLOT(close()));
	QObject::connect(m_settingsView, SIGNAL(quit()), SLOT(close()));

	m_settingsView->engine()->addImageProvider("state", new StateImageProvider(m_stateListModel));
	QDeclarativeContext *context = m_settingsView->rootContext();
	context->setContextProperty("machineView", static_cast<QObject *>(this));
	context->setContextProperty("machine", static_cast<QObject *>(m_machine));
	context->setContextProperty("stateListModel", static_cast<QObject *>(m_stateListModel));
}

// two-stage pause preventing deadlocks
void MachineView::pause() {
	if (!m_running || m_pauseRequested)
		return;
	m_closeTries = 0;
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
	if (m_thread->isRunning()) {
		if (m_quit) {
			m_closeTries++;
			if (m_closeTries > 400) {
				m_thread->terminate();
				close();
				return;
			}
		}
		QTimer::singleShot(10, this, SLOT(pauseStage2()));
	}
	m_pauseRequested = false;
	m_running = false;
	if (!m_quit) {
		if (m_settingsView->source().isEmpty() || !m_error.isEmpty()) {
			QString path;
			if (m_error.isEmpty())
				path = "%1/qml/base/main.qml";
			else
				path = "%1/qml/base/error.qml";
			path = path.arg(PathManager::instance()->installationDirPath());
			QUrl url = QUrl::fromLocalFile(path);
			m_settingsView->setSource(url);
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
	Q_ASSERT(m_error.isEmpty());
	if (m_running)
		return;
	m_hostVideo->setMyVisible(true);
	m_settingsView->setMyVisible(false);

	QObject::connect(m_thread, SIGNAL(frameGenerated(bool)),
					 this, SLOT(onFrameGenerated(bool)),
					 Qt::BlockingQueuedConnection);
	if (m_audioEnable)
		m_hostAudio->open();
	m_running = true;
	// use delay to wait for animation end
	QTimer::singleShot(500, m_thread, SLOT(resume()));
}

bool MachineView::close() {
	m_quit = true;
	if (m_running) {
		pause();
		return false;
	} else {
		qApp->quit();
		return true;
	}
}

void MachineView::saveScreenShotIfNotExists() {
	QString diskTitle = QFileInfo(m_diskFileName).completeBaseName();
	QString path = PathManager::instance()->screenShotPath(diskTitle);
	if (!QFile::exists(path))
		saveScreenShot();
}

void MachineView::saveScreenShot() {
	QImage img = m_machine->frame().copy(m_machine->videoSrcRect().toRect());
	img = img.convertToFormat(QImage::Format_ARGB32);
	QString diskTitle = QFileInfo(m_diskFileName).completeBaseName();
	img.save(PathManager::instance()->screenShotPath(diskTitle));
	QByteArray ba;
	QDataStream s(&ba, QIODevice::WriteOnly);
	s << m_diskFileName;
	QUdpSocket sock;
	sock.writeDatagram(ba, QHostAddress::LocalHost, 5798);
}

void MachineView::loadSettings() {
	QSettings s;
	m_hostVideo->setSwipeEnabled(s.value("swipeEnable", false).toBool());
	m_hostInput->setPadOpacity(loadOptionFromSettings(s, "padOpacity", 0.45f).toReal());
	m_thread->setFrameSkip(loadOptionFromSettings(s, "frameSkip", 1).toInt());
	m_hostVideo->setFpsVisible(loadOptionFromSettings(s, "fpsVisible", false).toBool());
	m_hostVideo->setKeepAspectRatio(loadOptionFromSettings(s, "keepAspectRatio", true).toBool());
}

QVariant MachineView::loadOptionFromSettings(QSettings &s,
											 const QString &name,
											 const QVariant &defaultValue) {
	QVariant option = m_machine->conf()->item(name);
	if (option.isNull())
		option = s.value(name, defaultValue);
	return option;
}

QString MachineView::extractArg(const QStringList &args,
								const QString &argName) {
	int optionArgIndex = args.indexOf(argName);
	if (optionArgIndex >= 0) {
		if (args.size() > optionArgIndex+1)
			return args.at(optionArgIndex+1);
	}
	return QString();
}

void MachineView::parseConfArg(const QString &arg) {
	QByteArray ba = QByteArray::fromBase64(arg.toAscii());
	QDataStream stream(&ba, QIODevice::ReadOnly);
	QMap<QString, QVariant> conf;
	stream >> conf;
	QMap<QString, QVariant>::ConstIterator i = conf.constBegin();
	for (; i != conf.constEnd(); i++)
		m_machine->conf()->setItem(i.key(), i.value());
}

void MachineView::onFrameGenerated(bool videoOn) {
	if (m_audioEnable)
		m_hostAudio->sendFrame();
	if (videoOn)
		m_hostVideo->repaint();
}

bool MachineView::isFpsVisible() const {
	return m_hostVideo->isFpsVisible();
}

void MachineView::setFpsVisible(bool on) {
	if (m_hostVideo->isFpsVisible() != on) {
		m_hostVideo->setFpsVisible(on);
		m_machine->conf()->setItem("fpsVisible", on);
		emit fpsVisibleChanged();
	}
}

int MachineView::frameSkip() const {
	return m_thread->frameSkip();
}

void MachineView::setFrameSkip(int n) {
	if (m_thread->frameSkip() != n) {
		m_thread->setFrameSkip(n);
		m_machine->conf()->setItem("frameSkip", n);
		emit frameSkipChanged();
	}
}

bool MachineView::isAudioEnabled() const {
	return m_audioEnable;
}

void MachineView::setAudioEnabled(bool on) {
	if (m_audioEnable != on) {
		m_audioEnable = on;
		m_machine->setAudioEnabled(on);
		m_machine->conf()->setItem("audioEnable", on);
		emit audioEnableChanged();
	}
}

qreal MachineView::padOpacity() const {
	return m_hostInput->padOpacity();
}

void MachineView::setPadOpacity(qreal opacity) {
	if (m_hostInput->padOpacity() != opacity) {
		m_hostInput->setPadOpacity(opacity);
		m_machine->conf()->setItem("padOpacity", opacity);
		emit padOpacityChanged();
	}
}

bool MachineView::keepAspectRatio() const {
	return m_hostVideo->keepApsectRatio();
}

void MachineView::setKeepAspectRatio(bool on) {
	if (m_hostVideo->keepApsectRatio() != on) {
		m_hostVideo->setKeepAspectRatio(on);
		m_machine->conf()->setItem("keepAspectRatio", on);
		emit keepAspectRatioChanged();
	}
}

int MachineView::determineLoadState(const QStringList &args) {
	int state = StateListModel::AutoSaveLoadSlot;
	if (args.contains("-noAutoSaveLoad")) {
		m_autoSaveLoadEnable = false;
		state = StateListModel::InvalidSlot;
	} else {
		QString stateArg = extractArg(args, "-state");
		if (!stateArg.isEmpty())
			state = stateArg.toInt();
	}
	if (state == StateListModel::AutoSaveLoadSlot) {
		if (!m_stateListModel->exists(StateListModel::AutoSaveLoadSlot))
			state = StateListModel::InvalidSlot;
	}
	return state;
}

bool MachineView::loadConfiguration() {
	Configuration *conf = m_machine->conf();
	conf->setItem("version", QCoreApplication::applicationVersion());

	QStringList args = QCoreApplication::arguments();

	int state = determineLoadState(args);
	m_thread->setLoadSlot(state);

	// load conf from state
	if (state != StateListModel::InvalidSlot) {
		emsl.loadConfOnly = true;
		if (!m_stateListModel->loadState(state))
			return false;
		emsl.loadConfOnly = false;
	}

	// load conf from arg
	QString confArg = extractArg(args, "-conf");
	parseConfArg(confArg);

	// load conf from global settings
	loadSettings();

	if (!conf->item("audioEnable", true).toBool())
		m_machine->setAudioEnabled(false);

	return true;
}

void MachineView::onSlFailed() {
	if (!emsl.save && emsl.abortIfLoadFails) {
		fatalError(constructSlErrorString());
	} else {
		emit faultOccured(constructSlErrorString());
	}
}

QString MachineView::constructSlErrorString() const {
	QString result;
	if (emsl.save)
		result = tr("Saving state failed: ");
	else
		result = tr("Loading state failed: ");
	result += emsl.error;
	return result;
}

void MachineView::fatalError(const QString &errorStr) {
	m_error = errorStr;
	if (m_running)
		pause();
	else
		pauseStage2();
}

QList<QObject *> MachineView::inputDevices() const {
	return m_hostInput->devices();
}

QDeclarativeView *MachineView::settingsView() const {
	return m_settingsView;
}
