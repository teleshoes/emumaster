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

#include "hostinput.h"
#include "imachine.h"
#include <QKeyEvent>
#include <QTouchEvent>

HostInput::HostInput(IMachine *machine) :
	m_machine(machine) {
	m_keysPhone = 0;
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
		m_keysPhone |=  key;
	else
		m_keysPhone &= ~key;
}

void HostInput::processKey(Qt::Key key, bool state) {
	int lastKeys = m_keysPhone;
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
	case Qt::Key_G:		setKeyState(IMachine::L_PadKey, state); break;
	case Qt::Key_H:		setKeyState(IMachine::R_PadKey, state); break;
	case Qt::Key_T:		setKeyState(IMachine::L2_PadKey, state); break;
	case Qt::Key_Z:		setKeyState(IMachine::R2_PadKey, state); break;
	case Qt::Key_W:		setKeyState(IMachine::Select_PadKey, state); break;
	case Qt::Key_Escape:emit pauseClicked(); break;
	default: break;
	}
	if (m_keysPhone != lastKeys)
		m_machine->setPadKeys(0, m_keysPhone);
}

void HostInput::processTouch(QEvent *e) {
	int lastKeys = m_keysPhone;
	m_keysPhone = 0;
	QTouchEvent *touchEvent = static_cast<QTouchEvent *>(e);
	QList<QTouchEvent::TouchPoint> points = touchEvent->touchPoints();
	for (int i = 0; i < points.size(); i++) {
		QTouchEvent::TouchPoint point = points.at(i);
		if (point.state() & Qt::TouchPointReleased)
			continue;
		int x = point.pos().x();
		int y = point.pos().y();
		if (x < 240) {
			if (y >= 240) {
				y -= 240;
				if (x < 60) {
					setKeyState(IMachine::Left_PadKey, true);
					if (y < 60)
						setKeyState(IMachine::Up_PadKey, true);
					else if (y >= 180)
						setKeyState(IMachine::Down_PadKey, true);
				} else if (x >= 180) {
					setKeyState(IMachine::Right_PadKey, true);
					if (y < 60)
						setKeyState(IMachine::Up_PadKey, true);
					else if (y >= 180)
						setKeyState(IMachine::Down_PadKey, true);
				} else {
					if (y < 60)
						setKeyState(IMachine::Up_PadKey, true);
					else if (y >= 180)
						setKeyState(IMachine::Down_PadKey, true);
					else {
						x -= 120;
						y -= 120;
						if (qAbs(x) > qAbs(y)) {
							if (x > 0)
								setKeyState(IMachine::Right_PadKey, true);
							else
								setKeyState(IMachine::Left_PadKey, true);
						} else {
							if (y > 0)
								setKeyState(IMachine::Down_PadKey, true);
							else
								setKeyState(IMachine::Up_PadKey, true);
						}
					}
				}
			} else {
				if (x < 90) {
					if (y < 40)
						setKeyState(IMachine::Select_PadKey, true);
					else if (y >= 110 && y < 170)
						setKeyState(IMachine::L_PadKey, true);
				} else if (x >= 150 && y < 40) {
					emit wantClose();
				}
			}
		} else if (x >= 854-240) {
			x -= 854-240;
			if (y >= 240) {
				y -= 240;
				if (x < 60) {
					setKeyState(IMachine::Y_PadKey, true);
					if (y < 60)
						setKeyState(IMachine::X_PadKey, true);
					else if (y >= 180)
						setKeyState(IMachine::B_PadKey, true);
				} else if (x >= 180) {
					setKeyState(IMachine::A_PadKey, true);
					if (y < 60)
						setKeyState(IMachine::X_PadKey, true);
					else if (y >= 180)
						setKeyState(IMachine::B_PadKey, true);
				} else {
					if (y < 60)
						setKeyState(IMachine::X_PadKey, true);
					else if (y >= 180)
						setKeyState(IMachine::B_PadKey, true);
					else {
						x -= 120;
						y -= 120;
						if (qAbs(x) > qAbs(y)) {
							if (x > 0)
								setKeyState(IMachine::A_PadKey, true);
							else
								setKeyState(IMachine::Y_PadKey, true);
						} else {
							if (y > 0)
								setKeyState(IMachine::B_PadKey, true);
							else
								setKeyState(IMachine::X_PadKey, true);
						}
					}
				}
			} else {
				if (x >= 150) {
					if (y < 40)
						setKeyState(IMachine::Start_PadKey, true);
					else if (y >= 110 && y < 170)
						setKeyState(IMachine::R_PadKey, true);
				} else if (x < 90 && y < 40) {
					emit pauseClicked();
				}
			}
		}
	}
	if (m_keysPhone != lastKeys)
		m_machine->setPadKeys(0, m_keysPhone);
}

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
	int lastKeys = m_keysPhone;
	setKeyState(IMachine::Up_PadKey, z > 9.8f/3);
	setKeyState(IMachine::Down_PadKey, z < -9.8f/3);

	setKeyState(IMachine::Right_PadKey, y > 9.8f/3);
	setKeyState(IMachine::Left_PadKey, y < -9.8f/3);

	if (m_keysPhone != lastKeys)
		m_machine->setPadKeys(0, m_keysPhone);
}

/*static const int sixAxisMapping[] = {
	IMachine::Select_PadKey,
	0,
	0,
	IMachine::Start_PadKey,
	IMachine::Up_PadKey,
	IMachine::Right_PadKey,
	IMachine::Down_PadKey,
	IMachine::Left_PadKey,
	IMachine::L2_PadKey,
	IMachine::R2_PadKey,
	IMachine::L_PadKey,
	IMachine::R_PadKey,
	IMachine::X_PadKey,
	IMachine::A_PadKey,
	IMachine::B_PadKey,
	IMachine::Y_PadKey
};

void HostInput::sixAxisChanged(int n, SixAxis *sixAxis) {
	if (!m_running)
		return;
	int b = sixAxis->buttons();
	if (b & SixAxis::PS) {
		emit pauseClicked();
		return;
	}
	int keys = 0;
	for (uint i = 0; i < sizeof(sixAxisMapping)/sizeof(int); i++) {
		if (b & (1 << i))
			keys |= sixAxisMapping[i];
	}
	m_machine->setPadKeys(n, keys);
}*/
