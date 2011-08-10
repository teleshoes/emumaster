#include "nespad.h"
#include <QDebug>

NesPad::NesPad(QObject *parent) :
	QObject(parent),
	m_pointer(0) {
}

void NesPad::setButtonState(Button button, bool state) {
	if (state)
		m_buttons |= button;
	else
		m_buttons &= ~button;
}

quint8 NesPad::read() {
	quint8 ret = 0;
	if (m_pointer < 8) {
		ret = m_buttons & static_cast<Button> (1 << m_pointer);
		m_pointer++;
	}
	return ret ? 1 : 0;
}

void NesPad::reset()
{ m_pointer = 0; }
