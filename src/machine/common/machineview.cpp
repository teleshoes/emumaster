#include "machineview.h"
#include "imachine.h"
#include "machinethread.h"
#include "hostvideo.h"
#include "hostaudio.h"
#include "hostinput.h"
#include "machineimageprovider.h"
#include <QDeclarativeView>
#include <QDeclarativeContext>
#include <QCloseEvent>
#include <QApplication>
#include <QDeclarativeEngine>
#include <QTimer>

#if defined(MEEGO_EDITION_HARMATTAN)
#include <QX11Info>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#endif

MachineView::MachineView(IMachine *machine, const QString &diskName, QWidget *parent) :
	QWidget(parent),
	m_machine(machine),
	m_running(false),
	m_backgroundCounter(0),
	m_diskName(diskName),
	m_wantClose(false) {
	Q_ASSERT(m_machine != 0);

#if defined(MEEGO_EDITION_HARMATTAN)
	showFullScreen();
#else
	resize(854, 480);
	show();
#endif

	m_thread = new MachineThread(this);

	m_hostAudio = new HostAudio(this);
	m_hostVideo = new HostVideo(this);
	m_hostInput = new HostInput(this);

	m_hostVideo->installEventFilter(m_hostInput);
	m_hostVideo->m_srcRect = m_machine->videoSrcRect();
	m_hostVideo->m_dstRect = m_machine->videoDstRect();

	m_settingsView = new QDeclarativeView(this);
	m_settingsView->engine()->addImageProvider("machine", new MachineImageProvider(this));
	m_settingsView->rootContext()->setContextProperty("backgroundPath", "");
	m_settingsView->rootContext()->setContextProperty("machineView", static_cast<QObject *>(this));
	m_settingsView->rootContext()->setContextProperty("video", static_cast<QObject *>(m_hostVideo));
	m_settingsView->rootContext()->setContextProperty("audio", static_cast<QObject *>(m_hostAudio));
	m_settingsView->rootContext()->setContextProperty("input", static_cast<QObject *>(m_hostInput));
	m_settingsView->rootContext()->setContextProperty("machine", static_cast<QObject *>(m_machine));
	m_settingsView->resize(size());

	QString error = m_machine->setDisk(QString("/home/user/MyDocs/emumaster/%1/%2")
									   .arg(m_machine->name())
									   .arg(diskName));
	if (!error.isEmpty())
		showError(error);

	m_machine->moveToThread(m_thread);
	m_machine->updateSettings();

	QTimer::singleShot(100, this, SLOT(resume()));
}

MachineView::~MachineView() {
	if (m_thread->isRunning())
		m_thread->wait();
	delete m_machine;
}

void MachineView::setupSwipe(bool on) {
#if defined(MEEGO_EDITION_HARMATTAN)
	Display *dpy = QX11Info::display();
	Window w = effectiveWinId();

	unsigned long val = (on ? 1 : 0);
	Atom atom = XInternAtom(dpy, "_MEEGOTOUCH_CANNOT_MINIMIZE", false);
	if (!atom) {
		qWarning("Unable to obtain _MEEGOTOUCH_CANNOT_MINIMIZE.");
		return;
	}
	XChangeProperty(dpy,
					w,
					atom,
					XA_CARDINAL,
					32,
					PropModeReplace,
					reinterpret_cast<unsigned char *>(&val),
					1);
#else
	Q_UNUSED(on)
#endif
}

void MachineView::showError(const QString &text) {
	Q_ASSERT(!text.isEmpty());
	m_hostVideo->m_error = text;
	m_settingsView->hide();
	m_hostVideo->show();
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
	QString path = QString("image://machine/screenShotGrayscaled%1").arg(m_backgroundCounter++);
	m_settingsView->rootContext()->setContextProperty("backgroundPath", path);
	if (m_settingsView->source().isEmpty())
		m_settingsView->setSource(QUrl::fromLocalFile(QString("../qml/%1/main.qml").arg(m_machine->name())));
	m_settingsView->show();
	m_settingsView->setFocus();
	m_hostVideo->hide();
	setupSwipe(true);
	m_running = false;
	emit runningChanged();
	if (m_wantClose)
		close();
}

void MachineView::resume() {
	if (m_running)
		return;
	m_machine->m_audioEnable = m_hostAudio->isEnabled();
	m_machine->m_audioSampleRate = m_hostAudio->sampleRate();
	m_machine->m_audioStereoEnable = m_hostAudio->isStereoEnabled();
	m_machine->updateSettings();

	m_hostVideo->show();
	m_hostVideo->setFocus();
	m_settingsView->hide();
	setupSwipe(m_hostInput->isSwipeEnabled());

	QObject::connect(m_thread, SIGNAL(frameGenerated()),
					 m_hostVideo, SLOT(repaint()),
					 Qt::BlockingQueuedConnection);
	if (m_hostVideo->m_error.isEmpty())
		m_thread->resume();
	else
		m_hostVideo->repaint();
	m_running = true;
	emit runningChanged();
}

void MachineView::closeEvent(QCloseEvent *e) {
	m_wantClose = true;
	if (m_running) {
		pause();
	} else {
		e->accept();
	}
}

void MachineView::saveScreenShot() {
	m_machine->frame().copy(m_machine->videoSrcRect().toRect())
			.save(QString("../data/%1/%2.jpg")
				  .arg(m_machine->name())
				  .arg(m_diskName));
}
