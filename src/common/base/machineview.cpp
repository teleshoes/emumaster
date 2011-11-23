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
#include "machinethread.h"
#include "hostvideo.h"
#include "hostaudio.h"
#include "hostinput.h"
#include "settingsview.h"
#include "machineimageprovider.h"
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
	m_wantClose(false),
	m_pauseRequested(false),
	m_audioEnable(true) {
	Q_ASSERT(m_machine != 0);

	PathManager::instance()->setMachine(machine->name());

	m_thread = new MachineThread(m_machine);
	bool autoLoadOnStart = !qApp->arguments().contains("-noautoload");
	if (autoLoadOnStart)
		m_thread->setLoadSlot(StateListModel::AutoSlot);

	m_hostInput = new HostInput(m_machine);
	m_hostAudio = new HostAudio(m_machine);

	m_hostVideo = new HostVideo(m_machine, m_thread);
	m_hostVideo->installEventFilter(m_hostInput);
	QObject::connect(m_hostVideo, SIGNAL(wantClose()), SLOT(close()));
	QObject::connect(m_hostVideo, SIGNAL(minimized()), SLOT(pause()));

	QString error = m_machine->init();

	if (error.isEmpty()) {
		error = m_machine->setDisk(QString("%1/%2")
								   .arg(PathManager::instance()->diskDirPath())
								   .arg(m_diskFileName));
	}

	loadSettings();

	m_stateListModel = new StateListModel(m_machine, m_diskFileName);
	m_thread->setStateListModel(m_stateListModel);

	m_settingsView = new SettingsView();
	QObject::connect(m_settingsView->engine(), SIGNAL(quit()), SLOT(close()));
	QObject::connect(m_settingsView, SIGNAL(wantClose()), SLOT(close()));

	m_settingsView->engine()->addImageProvider("state", new MachineImageProvider(m_stateListModel));
	m_settingsView->rootContext()->setContextProperty("backgroundPath", "");
	m_settingsView->rootContext()->setContextProperty("machineView", static_cast<QObject *>(this));
	m_settingsView->rootContext()->setContextProperty("machine", static_cast<QObject *>(m_machine));
	m_settingsView->rootContext()->setContextProperty("stateListModel", static_cast<QObject *>(m_stateListModel));

	if (!error.isEmpty())
		showError(error);
	else
		QObject::connect(m_hostInput, SIGNAL(pauseClicked()), SLOT(pause()));
	QObject::connect(m_hostInput, SIGNAL(wantClose()), SLOT(close()));
#if defined(MEEGO_EDITION_HARMATTAN)
	QMetaObject::invokeMethod(this, "resume", Qt::QueuedConnection);
#elif defined(Q_WS_MAEMO_5)
	QMetaObject::invokeMethod(this, "pauseStage2", Qt::QueuedConnection);
#endif
}

MachineView::~MachineView() {
	if (m_hostVideo->m_error.isEmpty()) {
		if (m_thread->isRunning())
			m_thread->wait();

		saveSettings();
		m_stateListModel->saveState(StateListModel::AutoSlot);

		// auto save screenshot
		QString title = QFileInfo(m_diskFileName).completeBaseName();
		QString path = PathManager::instance()->screenShotPath(title);
		if (!QFile::exists(path))
			saveScreenShot();

		m_machine->shutdown();
	}
	delete m_thread;
	delete m_settingsView;
	delete m_stateListModel;
	delete m_hostVideo;
	delete m_hostAudio;
	delete m_hostInput;
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
		if (m_wantClose) {
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
	if (!m_wantClose) {
		if (m_settingsView->source().isEmpty()) {
			QUrl url = QUrl::fromLocalFile(QString("%1/qml/base/main.qml")
										   .arg(PathManager::instance()->installationDirPath()));
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
	if (m_running)
		return;
	m_hostVideo->setMyVisible(true);
	m_settingsView->setMyVisible(false);

	if (m_hostVideo->m_error.isEmpty()) {
		QObject::connect(m_thread, SIGNAL(frameGenerated(bool)),
						 this, SLOT(onFrameGenerated(bool)),
						 Qt::BlockingQueuedConnection);
		if (m_audioEnable)
			m_hostAudio->open();
		m_running = true;
		// use delay to wait for animation end
		QTimer::singleShot(500, m_thread, SLOT(resume()));
	} else {
		m_hostVideo->repaint();
	}
}

bool MachineView::close() {
	m_wantClose = true;
	if (m_running) {
		pause();
		return false;
	} else {
		qApp->quit();
		return true;
	}
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

void MachineView::saveSettings() {
	QSettings s("elemental", "emumaster");
	s.setValue("swipeEnable", m_hostVideo->isSwipeEnabled());

	s.setValue("audioEnable", m_audioEnable);

	s.setValue("fpsVisible", m_hostVideo->isFpsVisible());
	s.setValue("keepAspectRatio", m_hostVideo->keepApsectRatio());
	s.setValue("padOpacity", padOpacity());

	s.beginGroup(m_machine->name());
	s.setValue("frameSkip", m_thread->frameSkip());

	QStringList accelDisks = s.value("accelerometerEnabledDisks", QStringList()).toStringList();
	bool accelDisksChanged = false;
	if (m_hostInput->isAccelerometerEnabled()) {
		if (!accelDisks.contains(m_diskFileName)) {
			accelDisks.append(m_diskFileName);
			accelDisksChanged = true;
		}
	} else {
		if (accelDisks.contains(m_diskFileName)) {
			accelDisks.removeOne(m_diskFileName);
			accelDisksChanged = true;
		}
	}
	if (accelDisksChanged)
		s.setValue("accelerometerEnabledDisks", accelDisks);

	m_machine->saveSettings(s);
	s.endGroup();
}

void MachineView::loadSettings() {
	QSettings s("elemental", "emumaster");
	m_hostVideo->setSwipeEnabled(s.value("swipeEnable", false).toBool());

	m_audioEnable = s.value("audioEnable", true).toBool();
	m_machine->setAudioEnabled(m_audioEnable);

	m_hostVideo->setFpsVisible(s.value("fpsVisible", false).toBool());
	m_hostVideo->setKeepAspectRatio(s.value("keepAspectRatio", true).toBool());
	m_hostVideo->setPadOpacity(s.value("padOpacity", 0.45f).toReal());

	s.beginGroup(m_machine->name());
	m_thread->setFrameSkip(s.value("frameSkip", 1).toInt());

	QStringList accelDisks = s.value("accelerometerEnabledDisks", QStringList()).toStringList();
	if (accelDisks.contains(m_diskFileName))
		m_hostInput->setAccelerometerEnabled(true);

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
bool MachineView::isSwipeEnabled() const
{ return m_hostVideo->isSwipeEnabled(); }
qreal MachineView::padOpacity() const
{ return m_hostVideo->padOpacity(); }
bool MachineView::keepAspectRatio() const
{ return m_hostVideo->keepApsectRatio(); }
bool MachineView::isAccelerometerEnabled() const
{ return m_hostInput->isAccelerometerEnabled(); }

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

void MachineView::setSwipeEnabled(bool on) {
	if (m_hostVideo->isSwipeEnabled() != on) {
		m_hostVideo->setSwipeEnabled(on);
		emit swipeEnableChanged();
	}
}

void MachineView::setPadOpacity(qreal opacity) {
	if (m_hostVideo->padOpacity() != opacity) {
		m_hostVideo->setPadOpacity(opacity);
		emit padOpacityChanged();
	}
}

void MachineView::setKeepAspectRatio(bool on) {
	if (m_hostVideo->keepApsectRatio() != on) {
		m_hostVideo->setKeepAspectRatio(on);
		emit keepAspectRatioChanged();
	}
}

QDeclarativeView *MachineView::settingsView() const
{ return m_settingsView; }

void MachineView::setAccelerometerEnabled(bool on) {
	if (m_hostInput->isAccelerometerEnabled() != on) {
		m_hostInput->setAccelerometerEnabled(on);
		emit accelerometerEnableChanged();
	}
}
