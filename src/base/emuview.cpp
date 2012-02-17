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

#include "emuview.h"
#include "emu.h"
#include "configuration.h"
#include "emuthread.h"
#include "hostvideo.h"
#include "hostaudio.h"
#include "hostinput.h"
#include "settingsview.h"
#include "stateimageprovider.h"
#include "statelistmodel.h"
#include "pathmanager.h"
#include "hostinputdevice.h"
#include <QDeclarativeView>
#include <QDeclarativeContext>
#include <qdeclarative.h>
#include <QCloseEvent>
#include <QApplication>
#include <QDeclarativeEngine>
#include <QTimer>
#include <QDir>
#include <QSettings>
#include <QUdpSocket>

EmuView::EmuView(Emu *emu, const QString &diskFileName) :
	m_emu(emu),
	m_diskFileName(diskFileName),
	m_running(false),
	m_backgroundCounter(qAbs(qrand())/2),
	m_quit(false),
	m_pauseRequested(false),
	m_audioEnable(true),
	m_autoSaveLoadEnable(true)
{
	Q_ASSERT(m_emu != 0);

	Configuration::setupAppInfo();
	registerClassesInQml();
	pathManager.setCurrentEmu(m_emu->name());

	m_thread = new EmuThread(m_emu);
	m_hostInput = new HostInput(m_emu);
	m_hostAudio = new HostAudio(m_emu);
	m_hostVideo = new HostVideo(m_hostInput, m_emu, m_thread);
	m_hostVideo->installEventFilter(m_hostInput);
	QObject::connect(m_hostVideo, SIGNAL(quit()), SLOT(close()));
	QObject::connect(m_hostInput, SIGNAL(quit()), SLOT(close()));
	QObject::connect(m_hostInput, SIGNAL(pause()), SLOT(pause()));
	QObject::connect(m_hostInput, SIGNAL(devicesChanged()),
					 SIGNAL(inputDevicesChanged()));

	m_stateListModel = new StateListModel(m_emu, m_diskFileName);
	m_thread->setStateListModel(m_stateListModel);

	// any config which modifies m_emu must be loaded later ...
	QSettings s;
	m_autoSaveLoadEnable = s.value("autoSaveLoadEnable").toBool();
	if (!loadConfiguration())
		m_error = constructSlErrorString();

	if (m_error.isEmpty()) {
		QString diskPath = QString("%1/%2")
				.arg(pathManager.diskDirPath())
				.arg(m_diskFileName);
		m_error = m_emu->init(diskPath);
	}
	if (m_error.isEmpty()) {
		// ... loaded here
		setAudioEnabled(loadOptionFromSettings(s, "audioEnable").toBool());
	}

	setupSettingsView();

	m_safetyTimer = new QTimer(this);
	m_safetyTimer->setInterval(10000);
	m_safetyTimer->setSingleShot(false);
	QObject::connect(m_safetyTimer, SIGNAL(timeout()), SLOT(onSafetyEvent()));

	const char *method = "resume";
	if (m_error.isEmpty()) {
		QObject::connect(m_stateListModel, SIGNAL(slFailed()),
						 SLOT(onSlFailed()), Qt::QueuedConnection);
		QObject::connect(m_stateListModel, SIGNAL(stateLoaded()),
						 SLOT(onStateLoaded()), Qt::QueuedConnection);
#if defined(Q_WS_MAEMO_5)
		method = "pauseStage2";
#endif
		QObject::connect(m_hostVideo, SIGNAL(focusOut()), SLOT(pause()));
	} else {
		method = "pauseStage2";
	}
	QMetaObject::invokeMethod(this, method, Qt::QueuedConnection);
}

EmuView::~EmuView()
{
	if (m_error.isEmpty()) {
		if (m_autoSaveLoadEnable)
			m_stateListModel->saveState(StateListModel::AutoSaveLoadSlot);
		// auto save screenshot
		saveScreenShotIfNotExists();
		m_emu->shutdown();
	}
	delete m_thread;
	delete m_settingsView;
	delete m_stateListModel;
	delete m_hostVideo;
	delete m_hostAudio;
	delete m_hostInput;
}

void EmuView::setupSettingsView()
{
	m_settingsView = new SettingsView();
	QObject::connect(m_settingsView->engine(), SIGNAL(quit()), SLOT(close()));
	QObject::connect(m_settingsView, SIGNAL(quit()), SLOT(close()));

	m_settingsView->engine()->addImageProvider("state", new StateImageProvider(m_stateListModel));
	QDeclarativeContext *context = m_settingsView->rootContext();
	context->setContextProperty("emuView", static_cast<QObject *>(this));
	context->setContextProperty("emu", static_cast<QObject *>(m_emu));
	context->setContextProperty("stateListModel", static_cast<QObject *>(m_stateListModel));
}

// two-stage pause preventing deadlocks
void EmuView::pause()
{
	if (!m_running || m_pauseRequested)
		return;
	m_closeTries = 0;
	m_pauseRequested = true;
	QObject::disconnect(m_thread, SIGNAL(frameGenerated(bool)),
						this, SLOT(onFrameGenerated(bool)));
	m_thread->pause();
	QMetaObject::invokeMethod(this, "pauseStage2", Qt::QueuedConnection);
}

void EmuView::pauseStage2()
{
	// the code below may be seen as bloat but it is needed
	// we are waiting for the thread to exit, but at the same
	// we allow blocking queued repaints from the thread
	if (m_thread->isRunning()) {
		m_closeTries++;
		if (m_closeTries > 40) {
			m_thread->terminate();
			m_error = tr("Emulated system is not responding.");
		} else {
			QTimer::singleShot(10, this, SLOT(pauseStage2()));
			return;
		}
	}
	m_pauseRequested = false;
	m_running = false;
	m_safetyTimer->stop();
	if (!m_quit) {
		if (m_settingsView->source().isEmpty() || !m_error.isEmpty()) {
			QString path;
			if (m_error.isEmpty())
				path = "%1/qml/base/main.qml";
			else
				path = "%1/qml/base/error.qml";
			path = path.arg(pathManager.installationDirPath());
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

void EmuView::resume()
{
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

	m_safetyCheck = false;
	m_safetyTimer->start();

	m_running = true;
	// use delay to wait for animation end
	QTimer::singleShot(500, m_thread, SLOT(resume()));
}

bool EmuView::close()
{
	m_quit = true;
	if (m_running) {
		pause();
		return false;
	} else {
		qApp->quit();
		return true;
	}
}

void EmuView::saveScreenShotIfNotExists()
{
	QString diskTitle = QFileInfo(m_diskFileName).completeBaseName();
	QString path = pathManager.screenShotPath(diskTitle);
	if (!QFile::exists(path))
		saveScreenShot();
}

void EmuView::saveScreenShot()
{
	QImage img = m_emu->frame().copy(m_emu->videoSrcRect().toRect());
	img = img.convertToFormat(QImage::Format_ARGB32);
	QString diskTitle = QFileInfo(m_diskFileName).completeBaseName();
	img.save(pathManager.screenShotPath(diskTitle));
	QByteArray ba;
	QDataStream s(&ba, QIODevice::WriteOnly);
	s << m_diskFileName;
	QUdpSocket sock;
	sock.writeDatagram(ba, QHostAddress::LocalHost, 5798);
}

void EmuView::loadSettings()
{
	QSettings s;
	m_hostVideo->setSwipeEnabled(loadOptionFromSettings(s, "swipeEnable").toBool());
	m_hostInput->setPadOpacity(loadOptionFromSettings(s, "padOpacity").toReal());
	m_thread->setFrameSkip(loadOptionFromSettings(s, "frameSkip").toInt());
	m_hostVideo->setFpsVisible(loadOptionFromSettings(s, "fpsVisible").toBool());
	m_hostVideo->setKeepAspectRatio(loadOptionFromSettings(s, "keepAspectRatio").toBool());
	m_hostVideo->setBilinearFiltering(loadOptionFromSettings(s, "bilinearFiltering").toBool());
	if (!loadOptionFromSettings(s, "runInBackground").toBool())
		QObject::connect(m_hostVideo, SIGNAL(minimized()), SLOT(pause()));
}

QVariant EmuView::loadOptionFromSettings(QSettings &s, const QString &name) const
{
	QVariant option = emConf.value(name);
	if (option.isNull())
		option = s.value(name, emConf.defaultValue(name));
	return option;
}

QString EmuView::extractArg(const QStringList &args,
								const QString &argName)
{
	int optionArgIndex = args.indexOf(argName);
	if (optionArgIndex >= 0) {
		if (args.size() > optionArgIndex+1)
			return args.at(optionArgIndex+1);
	}
	return QString();
}

void EmuView::parseConfArg(const QString &arg)
{
	QStringList lines = arg.split(',', QString::SkipEmptyParts);
	foreach (QString line, lines) {
		QStringList lineSplitted = line.split('=', QString::SkipEmptyParts);
		if (lineSplitted.size() != 2) {
			qDebug("Unknown conf option: %s", qPrintable(line));
		} else {
			QString key = lineSplitted.at(0);
			QString value = lineSplitted.at(1);
			emConf.setValue(key, value);
		}
	}
}

void EmuView::onFrameGenerated(bool videoOn)
{
	m_safetyCheck = true;
	if (m_audioEnable)
		m_hostAudio->sendFrame();
	if (videoOn)
		m_hostVideo->repaint();
	// sync input with the emulation
	m_hostInput->update();
}

bool EmuView::isFpsVisible() const
{
	return m_hostVideo->isFpsVisible();
}

void EmuView::setFpsVisible(bool on)
{
	if (m_hostVideo->isFpsVisible() != on) {
		m_hostVideo->setFpsVisible(on);
		emConf.setValue("fpsVisible", on);
		emit fpsVisibleChanged();
	}
}

int EmuView::frameSkip() const
{
	return m_thread->frameSkip();
}

void EmuView::setFrameSkip(int n)
{
	if (m_thread->frameSkip() != n) {
		m_thread->setFrameSkip(n);
		emConf.setValue("frameSkip", n);
		emit frameSkipChanged();
	}
}

bool EmuView::isAudioEnabled() const
{
	return m_audioEnable;
}

void EmuView::setAudioEnabled(bool on)
{
	if (m_audioEnable != on) {
		m_audioEnable = on;
		m_emu->setAudioEnabled(on);
		emConf.setValue("audioEnable", on);
		emit audioEnableChanged();
	}
}

qreal EmuView::padOpacity() const
{
	return m_hostInput->padOpacity();
}

void EmuView::setPadOpacity(qreal opacity)
{
	if (m_hostInput->padOpacity() != opacity) {
		m_hostInput->setPadOpacity(opacity);
		emConf.setValue("padOpacity", opacity);
		emit padOpacityChanged();
	}
}

bool EmuView::keepAspectRatio() const
{
	return m_hostVideo->keepApsectRatio();
}

void EmuView::setKeepAspectRatio(bool on)
{
	if (m_hostVideo->keepApsectRatio() != on) {
		m_hostVideo->setKeepAspectRatio(on);
		emConf.setValue("keepAspectRatio", on);
		emit keepAspectRatioChanged();
	}
}

bool EmuView::bilinearFiltering() const
{
	return m_hostVideo->bilinearFiltering();
}

void EmuView::setBilinearFiltering(bool enabled)
{
	if (m_hostVideo->bilinearFiltering() != enabled) {
		m_hostVideo->setBilinearFiltering(enabled);
		emConf.setValue("bilinearFiltering", enabled);
		emit bilinearFilteringChanged();
	}
}

int EmuView::determineLoadState(const QStringList &args)
{
	int state = StateListModel::AutoSaveLoadSlot;
	if (args.contains("-noAutoSaveLoad"))
		m_autoSaveLoadEnable = false;

	if (!m_autoSaveLoadEnable) {
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

bool EmuView::loadConfiguration()
{
	emConf.setValue("version", QCoreApplication::applicationVersion());

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

	return true;
}

void EmuView::onSlFailed()
{
	if (!emsl.save && emsl.abortIfLoadFails) {
		fatalError(constructSlErrorString());
	} else {
		emit faultOccured(constructSlErrorString());
	}
}

QString EmuView::constructSlErrorString() const
{
	QString result;
	if (emsl.save)
		result = tr("Saving state failed: ");
	else
		result = tr("Loading state failed: ");
	result += emsl.error;
	return result;
}

void EmuView::fatalError(const QString &errorStr)
{
	m_error = errorStr;
	if (m_running)
		pause();
	else
		pauseStage2();
}

void EmuView::registerClassesInQml()
{
	qmlRegisterType<HostInputDevice>();
}

QList<HostInputDevice *> EmuView::inputDevices() const
{
	return m_hostInput->devices();
}

void EmuView::onSafetyEvent()
{
	if (!m_safetyCheck) {
		m_thread->terminate();
		fatalError(tr("Emulated system is not responding"));
	}
	m_safetyCheck = false;
}

void EmuView::onStateLoaded()
{
	m_hostInput->loadFromConf();
}
