#include "gbapad.h"

#define REG_P1 0x098

extern "C" quint16 io_registers[1024 * 16];

GBAPad::GBAPad(QObject *parent) :
	QObject(parent),
	m_keys(0) {
}

void GBAPad::setKey(PadKey key, bool on) {
	if (on)
		m_keys |= key;
	else
		m_keys &= ~key;
	io_registers[REG_P1] = (~m_keys) & 0x3FF;
}