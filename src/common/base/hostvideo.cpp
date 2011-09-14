#include "hostvideo.h"
#include "imachine.h"
#include "machinethread.h"
#include <QPainter>
#include <QKeyEvent>

#if defined(MEEGO_EDITION_HARMATTAN)
#include <QX11Info>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#endif

HostVideo::HostVideo(IMachine *machine, MachineThread *thread) :
	m_machine(machine),
	m_thread(thread) {

	updateRects();
	QObject::connect(machine, SIGNAL(videoSrcRectChanged()), SLOT(updateRects()));

	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_AcceptTouchEvents);
	setAttribute(Qt::WA_QuitOnClose, false);
	setAutoFillBackground(false);

	m_fpsVisible = false;
	m_fpsCount = 0;
	m_fpsCounter = 0;
	m_fpsCounterTime.start();

	m_padVisible = true;
	QString dirPath = QString("%1/data").arg(IMachine::installationDirPath());

	m_padArrowsImage.load(dirPath + "/pad_arrows.png");
	m_padButtonsImage.load(dirPath + "/pad_buttons.png");

	m_selectButtonImage.load(dirPath + "/pad_select.png");
	m_startButtonImage.load(dirPath + "/pad_start.png");
	m_pauseButtonImage.load(dirPath + "/pause.png");
	m_quitButtonImage.load(dirPath + "/quit.png");

	m_swipeEnabled = true;
	m_quickQuitVisible = false;
}

HostVideo::~HostVideo() {
}

void HostVideo::initializeGL()
{ glClearColor(0.0f, 0.0f, 0.0f, 1.0f); }

void HostVideo::paintEvent(QPaintEvent *) {
	QPainter painter;
	painter.begin(this);
	painter.fillRect(rect(), Qt::black);

	if (!m_error.isEmpty()) {
		QFont font = painter.font();
		font.setPointSize(12);
		font.setBold(true);
		painter.setFont(font);
		painter.setPen(Qt::red);
		painter.drawText(rect(), Qt::AlignCenter, m_error);
	} else {
		if (!m_thread->m_inFrameGenerated)
			return;
		painter.drawImage(m_dstRect, m_machine->frame(), m_srcRect);
		if (m_fpsVisible) {
			m_fpsCounter++;
			if (m_fpsCounterTime.elapsed() >= 1000) {
				m_fpsCounterTime.restart();
				m_fpsCount = m_fpsCounter;
				m_fpsCounter = 0;
			}
			QFont font = painter.font();
			font.setPointSize(12);
			painter.setFont(font);
			painter.setPen(Qt::white);
			painter.drawText(QRectF(0.0f, 0.0f, 100.0f, 40.0f),
							 Qt::AlignCenter,
							 QString("%1 FPS").arg(m_fpsCount));
		}
		if (m_padVisible) {
			painter.drawImage(QPoint(0, 480-200), m_padArrowsImage);
			painter.drawImage(QPoint(854-200, 480-200), m_padButtonsImage);
			painter.drawImage(QPoint(0, 100), m_selectButtonImage);
			painter.drawImage(QPoint(854-75, 100), m_startButtonImage);
		}
	}
	painter.drawImage(QPoint(854-32-20, 20), m_pauseButtonImage);
	if (m_quickQuitVisible)
		painter.drawImage(QPoint(20, 20), m_quitButtonImage);
	painter.end();
}

void HostVideo::setFpsVisible(bool on)
{ m_fpsVisible = on; }
void HostVideo::setPadVisible(bool on)
{ m_padVisible = on; }
void HostVideo::setQuickQuitVisible(bool on)
{ m_quickQuitVisible = on; }

void HostVideo::setSwipeEnabled(bool on) {
#if defined(MEEGO_EDITION_HARMATTAN)
/*	implementation below don't work for now - when you touch on the edge the screen is blocked
	Display *dpy = QX11Info::display();
	Window w = effectiveWinId();

	unsigned long val = (on ? 0 : 1);
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
					1);*/
	if (m_swipeEnabled == on)
		return;
	m_swipeEnabled = on;

	Window w = effectiveWinId();
	Display *dpy = QX11Info::display();
	Atom atom;

	uint customRegion[4];
	customRegion[0] = 0;
	customRegion[1] = 0;
	customRegion[2] = 854;
	customRegion[3] = 480;

	atom = XInternAtom(dpy, "_MEEGOTOUCH_CUSTOM_REGION", False);
	if (!on) {
		XChangeProperty(dpy, w,
						atom, XA_CARDINAL, 32, PropModeReplace,
						reinterpret_cast<unsigned char *>(&customRegion[0]), 4);
	} else {
		XDeleteProperty(dpy, w, atom);
	}
#else
	Q_UNUSED(on)
#endif
}

void HostVideo::setMyVisible(bool visible) {
	if (visible) {
#	if defined(MEEGO_EDITION_HARMATTAN)
		showFullScreen();
#	else
		resize(854, 480);
		QGLWidget::setVisible(true);
#	endif
		setFocus();
	} else {
		QGLWidget::setVisible(false);
	}
}

void HostVideo::closeEvent(QCloseEvent *e) {
	e->ignore();
	emit wantClose();
}

void HostVideo::changeEvent(QEvent *e) {
	QGLWidget::changeEvent(e);
	if (e->type() == QEvent::WindowStateChange && windowState().testFlag(Qt::WindowMinimized))
		emit minimized();
}

void HostVideo::updateRects() {
	m_srcRect = m_machine->videoSrcRect();
	Q_ASSERT_X(m_srcRect.width() != 0.0f && m_srcRect.height() != 0.0f, "HostVideo", "define source rect!");
	qreal scale = qMin(854.0f/m_srcRect.width(), 480.0f/m_srcRect.height());
	qreal w = m_srcRect.width() * scale;
	qreal h = m_srcRect.height() * scale;
	qreal x = 854.0f/2.0f-w/2.0f;
	qreal y = 480.0f/2.0f-h/2.0f;
	// TODO keep aspect ratio option
//	m_dstRect = QRectF(x, y, w, h);
	m_dstRect = QRectF(QPointF(), size());
}
