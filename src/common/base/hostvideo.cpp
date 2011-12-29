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

#include "hostvideo.h"
#include "imachine.h"
#include "machinethread.h"
#include "pathmanager.h"
#include "hostinput.h"
#include <QPainter>
#include <QKeyEvent>

#if defined(MEEGO_EDITION_HARMATTAN)
#include <QX11Info>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#endif

HostVideo::HostVideo(HostInput *hostInput, IMachine *machine, MachineThread *thread) :
	m_hostInput(hostInput),
	m_machine(machine),
	m_thread(thread),
	m_firstPaint(true)
{
	QObject::connect(m_machine, SIGNAL(videoSrcRectChanged()), SLOT(updateRects()));

	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_AcceptTouchEvents);
	setAttribute(Qt::WA_QuitOnClose, false);
	setAutoFillBackground(false);

	m_fpsVisible = false;
	m_fpsCount = 0;
	m_fpsCounter = 0;
	m_fpsCounterTime.start();

	m_swipeEnabled = true;
	m_keepAspectRatio = true;
	m_bilinearFiltering = false;
}

HostVideo::~HostVideo()
{
}

void HostVideo::paintEvent(QPaintEvent *)
{
	QPainter painter;
	painter.begin(this);
	if (m_keepAspectRatio || m_firstPaint) {
		painter.fillRect(rect(), Qt::black);
		m_firstPaint = false;
	}

	if (m_thread->m_inFrameGenerated) {
		if (m_bilinearFiltering)
			painter.setRenderHint(QPainter::SmoothPixmapTransform);
		painter.drawImage(m_dstRect, m_machine->frame(), m_srcRect);
		if (m_fpsVisible)
			paintFps(painter);
	}
	m_hostInput->paint(painter);
	painter.end();
	m_hostInput->update();
}

void HostVideo::paintFps(QPainter &painter)
{
	m_fpsCounter++;
	if (m_fpsCounterTime.elapsed() >= 1000) {
		m_fpsCounterTime.restart();
		m_fpsCount = m_fpsCounter;
		m_fpsCounter = 0;
	}
	QFont font = painter.font();
	font.setPointSize(12);
	painter.setFont(font);
	painter.setPen(Qt::red);
	painter.drawText(QRectF(80.0f, 0.0f, 100.0f, 60.0f),
					 Qt::AlignCenter,
					 QString("%1 FPS").arg(m_fpsCount));
}

void HostVideo::setFpsVisible(bool on)
{
	m_fpsVisible = on;
}

void HostVideo::setSwipeEnabled(bool on)
{
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

void HostVideo::setMyVisible(bool visible)
{
	if (visible) {
		showFullScreen();
		setFocus();
	} else {
		QGLWidget::setVisible(false);
	}
}

void HostVideo::closeEvent(QCloseEvent *e)
{
	e->ignore();
	emit quit();
}

void HostVideo::changeEvent(QEvent *e)
{
	QGLWidget::changeEvent(e);
	if (e->type() == QEvent::WindowStateChange && windowState().testFlag(Qt::WindowMinimized))
		emit minimized();
}

void HostVideo::updateRects()
{
	m_srcRect = m_machine->videoSrcRect();
	Q_ASSERT_X(m_srcRect.width() != 0.0f && m_srcRect.height() != 0.0f, "HostVideo", "Define source rect!");
	qreal ww = Width;
	qreal wh = Height;
	if (m_keepAspectRatio) {
		qreal scale = qMin(ww/m_srcRect.width(), wh/m_srcRect.height());
		qreal w = m_srcRect.width() * scale;
		qreal h = m_srcRect.height() * scale;
		qreal x = ww/2.0f-w/2.0f;
		qreal y = wh/2.0f-h/2.0f;
		m_dstRect = QRectF(x, y, w, h);
	} else {
		m_dstRect = QRectF(QPointF(), QSizeF(ww, wh));
	}
}

void HostVideo::setKeepAspectRatio(bool on)
{
	m_keepAspectRatio = on;
	if (m_srcRect.width() != 0.0f)
		updateRects();
}

void HostVideo::setBilinearFiltering(bool enabled)
{
	m_bilinearFiltering = enabled;
}
