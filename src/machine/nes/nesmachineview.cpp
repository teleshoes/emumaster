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
		painter.translate(342/4, 0);
		painter.drawImage(QPoint(), m_machine->ppu()->frame());
	} else {
		painter.setPen(Qt::red);
		painter.drawText(rect(), Qt::AlignCenter, m_error);
	}
	painter.end();
}

void NesMachineView::keyPressEvent(QKeyEvent *e) {
	if (!e->isAutoRepeat()) {
		NesPad *pad = m_machine->pad0();
		if (e->key() == Qt::Key_Up)
			pad->setButtonState(NesPad::Up, true);
		else if (e->key() == Qt::Key_Down)
			pad->setButtonState(NesPad::Down, true);
		else if (e->key() == Qt::Key_Left)
			pad->setButtonState(NesPad::Left, true);
		else if (e->key() == Qt::Key_Right)
			pad->setButtonState(NesPad::Right, true);
		else if (e->key() == Qt::Key_C)
			pad->setButtonState(NesPad::A, true);
		else if (e->key() == Qt::Key_X)
			pad->setButtonState(NesPad::B, true);
		else if (e->key() == Qt::Key_S)
			pad->setButtonState(NesPad::Start, true);
		else if (e->key() == Qt::Key_Space)
			pad->setButtonState(NesPad::Select, true);
	}
}

void NesMachineView::keyReleaseEvent(QKeyEvent *e) {
	if (!e->isAutoRepeat()) {
		NesPad *pad = m_machine->pad0();
		if (e->key() == Qt::Key_Up)
			pad->setButtonState(NesPad::Up, false);
		else if (e->key() == Qt::Key_Down)
			pad->setButtonState(NesPad::Down, false);
		else if (e->key() == Qt::Key_Left)
			pad->setButtonState(NesPad::Left, false);
		else if (e->key() == Qt::Key_Right)
			pad->setButtonState(NesPad::Right, false);
		else if (e->key() == Qt::Key_C)
			pad->setButtonState(NesPad::A, false);
		else if (e->key() == Qt::Key_X)
			pad->setButtonState(NesPad::B, false);
		else if (e->key() == Qt::Key_S)
			pad->setButtonState(NesPad::Start, false);
		else if (e->key() == Qt::Key_Space)
			pad->setButtonState(NesPad::Select, false);
	}
}
