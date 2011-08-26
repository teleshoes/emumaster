#include "hostinput.h"
#include "machineview.h"
#include "imachine.h"
#include <QKeyEvent>
#include <QTouchEvent>

HostInput::HostInput(MachineView *parent) :
	QObject(parent),
	m_swipeEnabled(false),
	m_padVisible(true),
	m_machine(0) {
}

HostInput::~HostInput() {
}

void HostInput::setSwipeEnabled(bool on) {
	if (m_swipeEnabled != on) {
		m_swipeEnabled = on;
		emit swipeEnableChanged();
	}
}

void HostInput::setPadVisible(bool on) {
	if (m_padVisible != on) {
		m_padVisible = on;
		emit padVisibleChanged();
	}
}

bool HostInput::eventFilter(QObject *o, QEvent *e) {
	Q_UNUSED(o)
	if (!m_machine) {
		MachineView *machineView = static_cast<MachineView *>(parent());
		m_machine = machineView->machine();
		if (!m_machine)
			return false;
	}
	if (e->type() == QEvent::KeyPress || e->type() == QKeyEvent::KeyRelease) {
		bool state = (e->type() == QEvent::KeyPress);
		QKeyEvent *ke = static_cast<QKeyEvent *>(e);
		if (!ke->isAutoRepeat())
			processKey(static_cast<Qt::Key>(ke->key()), state);
		return true;
	} else if (e->type() == QEvent::TouchBegin || e->type() == QEvent::TouchUpdate || e->type() == QEvent::TouchEnd) {
		QTouchEvent *touchEvent = static_cast<QTouchEvent *>(e);
		if (e->type() == QEvent::TouchBegin)
			qDebug("TouchBegin");
		else if (e->type() == QEvent::TouchUpdate)
			qDebug("TouchUpdate");
		else
			qDebug("TouchEnd");
		QList<QTouchEvent::TouchPoint> points = touchEvent->touchPoints();
		for (int i = 0; i < points.size(); i++) {
			QTouchEvent::TouchPoint point = points.at(i);
			qDebug(qPrintable(QString("%1 %2").arg(point.pos().x()).arg(point.pos().y())));
			if (point.state() & Qt::TouchPointPressed)
				qDebug("pressed");
			if (point.state() & Qt::TouchPointMoved)
				qDebug("moved");
			if (point.state() & Qt::TouchPointStationary)
				qDebug("stationary");
			if (point.state() & Qt::TouchPointReleased)
				qDebug("released");
		}
		return true;
	} else {
		return false;
	}
}

void HostInput::processKey(Qt::Key key, bool state) {
	switch (key) {
	case Qt::Key_Left:	m_machine->setPadKey(IMachine::Left_PadKey, state); break;
	case Qt::Key_Right:	m_machine->setPadKey(IMachine::Right_PadKey, state); break;
	case Qt::Key_Up:	m_machine->setPadKey(IMachine::Up_PadKey, state); break;
	case Qt::Key_Down:	m_machine->setPadKey(IMachine::Down_PadKey, state); break;
	case Qt::Key_X:		m_machine->setPadKey(IMachine::A_PadKey, state); break;
	case Qt::Key_C:		m_machine->setPadKey(IMachine::B_PadKey, state); break;
	case Qt::Key_V:		m_machine->setPadKey(IMachine::C_PadKey, state); break;
	case Qt::Key_S:		m_machine->setPadKey(IMachine::X_PadKey, state); break;
	case Qt::Key_D:		m_machine->setPadKey(IMachine::Y_PadKey, state); break;
	case Qt::Key_F:		m_machine->setPadKey(IMachine::Z_PadKey, state); break;
	case Qt::Key_Q:		m_machine->setPadKey(IMachine::Start_PadKey, state); break;
	case Qt::Key_W:		m_machine->setPadKey(IMachine::Select_PadKey, state); break;
	default: break;
	}
}
