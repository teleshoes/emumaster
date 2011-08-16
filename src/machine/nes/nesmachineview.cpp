#include "nesmachineview.h"
#include "nesmachine.h"
#include "nesppu.h"
#include "nesdisk.h"
#include "nespad.h"
#include <QPainter>
#include <QKeyEvent>
#include <QDebug>

NesMachineView::NesMachineView(const QString &diskName, QWidget *parent) :
	QGLWidget(parent) {
// TODO Harmattan -> fullScreen()
	m_sourceRect = QRectF(8.0f, 0.0f, NesPpu::VisibleScreenWidth, NesPpu::VisibleScreenHeight);
	resize(854, 480);
	m_machine = new NesMachine(this);
	NesDisk *disk = new NesDisk(diskName, this);
	if (!m_machine->setDisk(disk)) {
		if (!disk->isLoaded())
			m_error = QString("Could not load ROM file\n%1").arg(diskName);
		else
			m_error = QString("Mapper %1 is not supported").arg(disk->mapperType());
	} else {
		QObject::connect(m_machine, SIGNAL(frameGenerated()), SLOT(repaint()));
		m_machine->setRunning(true);
		m_machine->reset();
	}
}

void NesMachineView::paintEvent(QPaintEvent *) {
	QPainter painter;
	painter.begin(this);
	if (m_error.isEmpty()) {
		painter.scale(2.0f, 2.0f);
		painter.drawImage(QPointF(342.0f/4.0f, 0.0f), m_machine->ppu()->frame(), m_sourceRect);
	} else {
		painter.setPen(Qt::red);
		painter.drawText(rect(), Qt::AlignCenter, m_error);
	}
	painter.end();
}

void NesMachineView::keyPressEvent(QKeyEvent *e) {
	if (!e->isAutoRepeat()) {
		NesPad *pad = m_machine->pad();
		if (e->key() == Qt::Key_Up)
			pad->setButtonState(0, NesPad::Up, true);
		else if (e->key() == Qt::Key_Down)
			pad->setButtonState(0, NesPad::Down, true);
		else if (e->key() == Qt::Key_Left)
			pad->setButtonState(0, NesPad::Left, true);
		else if (e->key() == Qt::Key_Right)
			pad->setButtonState(0, NesPad::Right, true);
		else if (e->key() == Qt::Key_C)
			pad->setButtonState(0, NesPad::A, true);
		else if (e->key() == Qt::Key_X)
			pad->setButtonState(0, NesPad::B, true);
		else if (e->key() == Qt::Key_S)
			pad->setButtonState(0, NesPad::Start, true);
		else if (e->key() == Qt::Key_Space)
			pad->setButtonState(0, NesPad::Select, true);
	}
}

void NesMachineView::keyReleaseEvent(QKeyEvent *e) {
	if (!e->isAutoRepeat()) {
		NesPad *pad = m_machine->pad();
		if (e->key() == Qt::Key_Up)
			pad->setButtonState(0, NesPad::Up, false);
		else if (e->key() == Qt::Key_Down)
			pad->setButtonState(0, NesPad::Down, false);
		else if (e->key() == Qt::Key_Left)
			pad->setButtonState(0, NesPad::Left, false);
		else if (e->key() == Qt::Key_Right)
			pad->setButtonState(0, NesPad::Right, false);
		else if (e->key() == Qt::Key_C)
			pad->setButtonState(0, NesPad::A, false);
		else if (e->key() == Qt::Key_X)
			pad->setButtonState(0, NesPad::B, false);
		else if (e->key() == Qt::Key_S)
			pad->setButtonState(0, NesPad::Start, false);
		else if (e->key() == Qt::Key_Space)
			pad->setButtonState(0, NesPad::Select, false);
	}
}
