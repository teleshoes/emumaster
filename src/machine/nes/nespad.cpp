#include "nespad.h"

NesPad::NesPad(QObject *parent) :
	QObject(parent) {
}

void NesPad::setButtonState(int player, Button button, bool on) {
	Q_ASSERT(player == 0 || player == 1);
	Buttons &pad = ((player == 0) ? m_padA : m_padB);
	if (on)
		pad |= button;
	else
		pad &= ~button;
}

void NesPad::reset() {
	m_padAReg = 0;
	m_padBReg = 0;
}

void NesPad::write(quint16 address, quint8 data) {
	Q_ASSERT(address < 2);
	if (address == 0) {
		if (data & 0x01) {
			m_nextStrobe = true;
		} else if (m_nextStrobe) {
			m_nextStrobe = false;
			strobe();
		}
	} else {
		// TODO expad
	}
}

quint8 NesPad::read(quint16 address) {
	Q_ASSERT(address < 2);
	quint8 data;
	if (address == 0) {
		data = m_padAReg & 1;
		m_padAReg >>= 1;
		// TODO expad
	} else {
		data = m_padBReg & 1;
		m_padBReg >>= 1;
		// TODO expad
	}
	return	data;
}

void NesPad::strobe() {
	m_padAReg = int(m_padA);
	m_padBReg = int(m_padB);
	// TODO expad
}
