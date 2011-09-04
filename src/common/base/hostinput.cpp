#include "hostinput.h"
#include "imachine.h"
#include <QKeyEvent>
#include <QTouchEvent>

HostInput::HostInput(IMachine *machine) :
	m_machine(machine) {
}

HostInput::~HostInput() {
}

bool HostInput::eventFilter(QObject *o, QEvent *e) {
	Q_UNUSED(o)
	if (e->type() == QEvent::KeyPress || e->type() == QKeyEvent::KeyRelease) {
		bool state = (e->type() == QEvent::KeyPress);
		QKeyEvent *ke = static_cast<QKeyEvent *>(e);
		if (!ke->isAutoRepeat())
			processKey(static_cast<Qt::Key>(ke->key()), state);
		return true;
	} else if (e->type() == QEvent::TouchBegin || e->type() == QEvent::TouchUpdate || e->type() == QEvent::TouchEnd) {
		processTouch(e);
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
	case Qt::Key_C:		m_machine->setPadKey(IMachine::A_PadKey, state); break;
	case Qt::Key_X:		m_machine->setPadKey(IMachine::B_PadKey, state); break;
	case Qt::Key_S:		m_machine->setPadKey(IMachine::X_PadKey, state); break;
	case Qt::Key_D:		m_machine->setPadKey(IMachine::Y_PadKey, state); break;
	case Qt::Key_Q:		m_machine->setPadKey(IMachine::Start_PadKey, state); break;
	case Qt::Key_W:		m_machine->setPadKey(IMachine::Select_PadKey, state); break;
	case Qt::Key_Escape:emit pause(); break;
	default: break;
	}
}

void HostInput::processTouch(QEvent *e) {
	m_machine->setPadKey(IMachine::AllKeys, false);
	uint keys = 0;
	QTouchEvent *touchEvent = static_cast<QTouchEvent *>(e);
	QList<QTouchEvent::TouchPoint> points = touchEvent->touchPoints();
	for (int i = 0; i < points.size(); i++) {
		QTouchEvent::TouchPoint point = points.at(i);
		if (point.state() & Qt::TouchPointReleased)
			continue;
		int x = point.pos().x();
		int y = point.pos().y();
		if (x < 854/2) {
			if (y >= 480-200) {
				if (x < 200) {
					y -= 480-200;
					if (x < 70 && y >= 30 && y < 200-30)
						keys |= IMachine::Left_PadKey;
					if (x >= 200-70 && y >= 30 && y < 200-30)
						keys |= IMachine::Right_PadKey;
					if (y < 70 && x >= 30 && x < 200-30)
						keys |= IMachine::Up_PadKey;
					if (y >= 200-70 && x >= 30 && x < 200-30)
						keys |= IMachine::Down_PadKey;
				}
			} else {
				if (x < 70 && y >= 100 && y < 135)
					keys |= IMachine::Select_PadKey;
			}
		} else {
			if (y >= 480-200) {
				if (x >= 854-200) {
					x -= 854-200;
					y -= 480-200;
					if (x < 70 && y >= 30 && y < 200-30)
						keys |= IMachine::Y_PadKey;
					if (x >= 200-70 && y >= 30 && y < 200-30)
						keys |= IMachine::A_PadKey;
					if (y < 70 && x >= 30 && x < 200-30)
						keys |= IMachine::X_PadKey;
					if (y >= 200-70 && x >= 30 && x < 200-30)
						keys |= IMachine::B_PadKey;
				}
			} else {
				if (x >= 854-70 && y >= 100 && y < 135)
					keys |= IMachine::Start_PadKey;
				if (x >= 854-70 && y < 70)
					emit pauseClicked();
			}
		}
	}
	if (keys)
		m_machine->setPadKey(static_cast<IMachine::PadKey>(keys), true);
}
