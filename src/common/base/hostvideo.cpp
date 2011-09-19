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

	QObject::connect(machine, SIGNAL(videoSrcRectChanged()), SLOT(updateRects()));

	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_AcceptTouchEvents);
	setAttribute(Qt::WA_QuitOnClose, false);
	setAutoFillBackground(false);

	m_fpsVisible = false;
	m_fpsCount = 0;
	m_fpsCounter = 0;
	m_fpsCounterTime.start();

	m_padOpacity = 0.45f;
	QString dirPath = QString("%1/data").arg(IMachine::installationDirPath());

	m_padLeftImage.load(dirPath + "/pad-left.png");
	if (machine->name() == "psx")
		m_padRightImage.load(dirPath + "/pad-right-psx.png");
	else
		m_padRightImage.load(dirPath + "/pad-right-abxy.png");

	m_swipeEnabled = true;
	m_keepAspectRatio = false;
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
		if (m_thread->m_inFrameGenerated) {
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
		}
	}
	if (m_padOpacity != 0.0f) {
		painter.setOpacity(m_padOpacity);
		painter.drawImage(QPoint(), m_padLeftImage);
		painter.drawImage(QPoint(854-240, 0), m_padRightImage);
	}
	painter.end();
}

void HostVideo::setFpsVisible(bool on)
{ m_fpsVisible = on; }
void HostVideo::setPadOpacity(qreal opacity)
{ m_padOpacity = opacity; }

void HostVideo::setSwipeEnabled(bool on) {
#if defined(MEEGO_EDITION_HARMATTAN)
	if (m_swipeEnabled == on)
		return;
	m_swipeEnabled = on;

	Window w = effectiveWinId();
	Display *dpy = QX11Info::display();
	Atom atom;

	uint customRegion[4];
	customRegion[0] = 0;
	customRegion[1] = 0;
	customRegion[2] = width();
	customRegion[3] = height();

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
		showFullScreen();
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
	Q_ASSERT_X(m_srcRect.width() != 0.0f && m_srcRect.height() != 0.0f, "HostVideo", "Define source rect!");
	if (m_keepAspectRatio) {
		qreal scale = qMin(qreal(width())/m_srcRect.width(), qreal(height())/m_srcRect.height());
		qreal w = m_srcRect.width() * scale;
		qreal h = m_srcRect.height() * scale;
		qreal x = qreal(width())/2.0f-w/2.0f;
		qreal y = qreal(height())/2.0f-h/2.0f;
		m_dstRect = QRectF(x, y, w, h);
	} else {
		m_dstRect = QRectF(QPointF(), size());
	}
}

void HostVideo::setKeepAspectRatio(bool on) {
	m_keepAspectRatio = on;
	updateRects();
}
