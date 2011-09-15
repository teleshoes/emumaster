#include "hostinput.h"
#include "imachine.h"
#include <QKeyEvent>
#include <QTouchEvent>

HostInput::HostInput(IMachine *machine) :
	m_machine(machine) {
	m_quickQuitEnabled = true;
	m_keys = 0;
	m_accelerometer = 0;
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
		ke->accept();
		return true;
	} else if (e->type() == QEvent::TouchBegin || e->type() == QEvent::TouchUpdate || e->type() == QEvent::TouchEnd) {
		processTouch(e);
		e->accept();
		return true;
	} else {
		return false;
	}
}

void HostInput::setKeyState(int key, bool state) {
	if (state)
		m_keys |=  key;
	else
		m_keys &= ~key;
}

void HostInput::processKey(Qt::Key key, bool state) {
	int lastKeys = m_keys;
	switch (key) {
	case Qt::Key_Left:	setKeyState(IMachine::Left_PadKey, state); break;
	case Qt::Key_Right:	setKeyState(IMachine::Right_PadKey, state); break;
	case Qt::Key_Up:	setKeyState(IMachine::Up_PadKey, state); break;
	case Qt::Key_Down:	setKeyState(IMachine::Down_PadKey, state); break;
	case Qt::Key_C:		setKeyState(IMachine::A_PadKey, state); break;
	case Qt::Key_X:		setKeyState(IMachine::B_PadKey, state); break;
	case Qt::Key_S:		setKeyState(IMachine::X_PadKey, state); break;
	case Qt::Key_D:		setKeyState(IMachine::Y_PadKey, state); break;
	case Qt::Key_Q:		setKeyState(IMachine::Start_PadKey, state); break;
	case Qt::Key_W:		setKeyState(IMachine::Select_PadKey, state); break;
	case Qt::Key_Escape:emit pauseClicked(); break;
	default: break;
	}
	if (m_keys != lastKeys)
		m_machine->setPadKeys(0, m_keys);
}

void HostInput::processTouch(QEvent *e) {
	int lastKeys = m_keys;
	m_keys = 0;
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
						setKeyState(IMachine::Left_PadKey, true);
					if (x >= 200-70 && y >= 30 && y < 200-30)
						setKeyState(IMachine::Right_PadKey, true);
					if (y < 70 && x >= 30 && x < 200-30)
						setKeyState(IMachine::Up_PadKey, true);
					if (y >= 200-70 && x >= 30 && x < 200-30)
						setKeyState(IMachine::Down_PadKey, true);
				}
			} else {
				if (x < 70 && y >= 100 && y < 135)
					setKeyState(IMachine::Select_PadKey, true);
				if (x < 60 && y < 60) {
					if (m_quickQuitEnabled)
						emit wantClose();
				}
			}
		} else {
			if (y >= 480-200) {
				if (x >= 854-200) {
					x -= 854-200;
					y -= 480-200;
					if (x < 70 && y >= 30 && y < 200-30)
						setKeyState(IMachine::Y_PadKey, true);
					if (x >= 200-70 && y >= 30 && y < 200-30)
						setKeyState(IMachine::A_PadKey, true);
					if (y < 70 && x >= 30 && x < 200-30)
						setKeyState(IMachine::X_PadKey, true);
					if (y >= 200-70 && x >= 30 && x < 200-30)
						setKeyState(IMachine::B_PadKey, true);
				}
			} else {
				if (x >= 854-70 && y >= 100 && y < 135)
					setKeyState(IMachine::Start_PadKey, true);
				if (x >= 854-60 && y < 60)
					emit pauseClicked();
			}
		}
	}
	if (m_keys != lastKeys)
		m_machine->setPadKeys(0, m_keys);
}

void HostInput::setQuickQuitEnabled(bool on)
{ m_quickQuitEnabled = on; }

bool HostInput::isAccelerometerEnabled() const
{ return m_accelerometer != 0; }

void HostInput::setAccelerometerEnabled(bool on) {
	if (on && m_accelerometer != 0)
		return;
	if (!on) {
		delete m_accelerometer;
		m_accelerometer = 0;
	} else {
		m_accelerometer = new QAccelerometer(this);
		QObject::connect(m_accelerometer, SIGNAL(readingChanged()), SLOT(accelerometerUpdated()));
		m_accelerometer->start();
	}
}

void HostInput::accelerometerUpdated() {
	QAccelerometerReading *reading = m_accelerometer->reading();
	qreal y = reading->y();
	qreal z = reading->z();
	int lastKeys = m_keys;
	setKeyState(IMachine::Up_PadKey, z > 9.8f/3);
	setKeyState(IMachine::Down_PadKey, z < -9.8f/3);

	setKeyState(IMachine::Right_PadKey, y > 9.8f/3);
	setKeyState(IMachine::Left_PadKey, y < -9.8f/3);

	if (m_keys != lastKeys)
		m_machine->setPadKeys(0, m_keys);
}
