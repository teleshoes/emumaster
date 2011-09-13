#include "pad.h"
#include "common.h"
#include "memory.h"

GbaPad::GbaPad(QObject *parent) :
	QObject(parent),
	m_keys(0) {
}

void GbaPad::setKey(PadKey key, bool on) {
	if (on)
		m_keys |= key;
	else
		m_keys &= ~key;
	io_registers[REG_P1] = (~m_keys) & 0x3FF;
}
