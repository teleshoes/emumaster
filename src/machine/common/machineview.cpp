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

#if defined(MEEGO_EDITION_HARMATTAN)
#include <QX11Info>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#endif

MachineView::MachineView(QWidget *parent) :
	QWidget(parent),
	m_machine(0),
	m_running(false),
	m_backgroundCounter(0) {

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

	m_settingsView = new QDeclarativeView(this);
	m_settingsView->engine()->addImageProvider("machine", new MachineImageProvider(this));
	m_settingsView->rootContext()->setContextProperty("backgroundPath", "");
	m_settingsView->rootContext()->setContextProperty("machineView", static_cast<QObject *>(this));
	m_settingsView->rootContext()->setContextProperty("video", static_cast<QObject *>(m_hostVideo));
	m_settingsView->rootContext()->setContextProperty("audio", static_cast<QObject *>(m_hostAudio));
	m_settingsView->rootContext()->setContextProperty("input", static_cast<QObject *>(m_hostInput));
	m_settingsView->resize(size());
	m_settingsView->show();
	m_hostVideo->hide();
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

void MachineView::setMachineAndQmlSettings(IMachine *m, const QString &qmlSettingsPath) {
	Q_ASSERT(m_machine == 0 && m != 0);
	m_machine = m;
	m_machine->moveToThread(m_thread);
	m_machine->updateSettings();
	m_settingsView->rootContext()->setContextProperty("machine", static_cast<QObject *>(m));
	m_settingsView->setSource(QUrl::fromLocalFile(qmlSettingsPath));
	resume();
}

void MachineView::setSourceRect(const QRectF &rect)
{ m_hostVideo->m_sourceRect = rect; }
void MachineView::setDestRect(const QRectF &rect)
{ m_hostVideo->m_destRect = rect; }

void MachineView::showError(const QString &text) {
	Q_ASSERT(!text.isEmpty());
	m_hostVideo->m_error = text;
	m_settingsView->hide();
	m_hostVideo->show();
}

void MachineView::pause() {
	if (!m_running)
		return;
	QObject::disconnect(m_thread, SIGNAL(frameGenerated()),
						m_hostVideo, SLOT(repaint()));
	m_thread->pause();
	m_thread->wait();
	QString path = QString("image://machine/screenShotGrayscaled%1").arg(m_backgroundCounter++);
	m_settingsView->rootContext()->setContextProperty("backgroundPath", path);
	m_settingsView->show();
	m_settingsView->setFocus();
	m_hostVideo->hide();
	setupSwipe(true);
	m_running = false;
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
	m_thread->resume();
	m_running = true;
}

void MachineView::closeEvent(QCloseEvent *e) {
	if (m_running) {
		e->ignore();
		m_thread->pause();
		m_running = m_thread->isRunning();
		QMetaObject::invokeMethod(this, "close", Qt::QueuedConnection);
	} else {
		e->accept();
	}
}
