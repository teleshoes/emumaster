#include "hostvideo.h"
#include "imachine.h"
#include "machineview.h"
#include "machinethread.h"
#include <QPainter>
#include <QKeyEvent>

HostVideo::HostVideo(MachineView *machineView) :
	m_machineView(machineView) {

	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_AcceptTouchEvents);
	setAutoFillBackground(false);

	grabGesture(Qt::PinchGesture);
	grabGesture(Qt::SwipeGesture);

	m_fpsVisble = true;
	m_fpsCount = 0;
	m_fpsCounter = 0;
	m_fpsCounterTime.start();

	m_frameSkip = 0;

	m_thread = machineView->m_thread;
}

HostVideo::~HostVideo() {
}

void HostVideo::setFpsVisible(bool on) {
	if (m_fpsVisble != on) {
		m_fpsVisble = on;
		emit fpsVisibleChanged();
	}
}

void HostVideo::setFrameSkip(int skip) {
	if (m_frameSkip != skip) {
		m_frameSkip = skip;
		emit frameSkipChanged();
	}
}

void HostVideo::initializeGL() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void HostVideo::paintEvent(QPaintEvent *) {
	IMachine *machine = m_machineView->m_machine;
	Q_ASSERT(machine != 0);

	QPainter painter;
	painter.begin(this);

	painter.beginNativePainting();
	glClear(GL_COLOR_BUFFER_BIT);
	painter.endNativePainting();

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
		painter.drawImage(m_dstRect, machine->frame(), m_srcRect);
		if (m_fpsVisble) {
			// TODO performance
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
	painter.end();
}

void HostVideo::mousePressEvent(QMouseEvent *me) {
	Q_UNUSED(me)
	m_machineView->pause();
}

QImage HostVideo::screenShotGrayscaled() const {
	IMachine *machine = m_machineView->m_machine;
	if (!machine)
		return QImage();
	QImage screenShot(size(), QImage::Format_RGB32);
	QPainter painter;
	painter.begin(&screenShot);
	painter.fillRect(QRectF(QPointF(0.0f, 0.0f), size()), qRgb(0xE0, 0xE1, 0xE2));
	painter.drawImage(m_dstRect, machine->frame(), m_srcRect);
	painter.end();

	int pixelCount = screenShot.width() * screenShot.height();
	QRgb *data = (QRgb *)screenShot.bits();
	for (int i = 0; i < pixelCount; ++i) {
		uint val = qGray(data[i]);
		data[i] = qRgb(val, val, val);
	}
	return screenShot;
}

void HostVideo::setVisible(bool visible) {
	if (visible) {
#	if defined(MEEGO_EDITION_HARMATTAN)
		showFullScreen();
#	else
		resize(854, 480);
		QGLWidget::setVisible(true);
#	endif
	} else {
		QGLWidget::setVisible(false);
	}
}
