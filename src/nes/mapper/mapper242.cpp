#include "mapper242.h"
#include "nesppu.h"
#include "nesdisk.h"

void Mapper242::reset() {
	NesMapper::reset();
	setRom32KBank(0);
}

void Mapper242::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(data)
	if (address & 0x01)
		setRom32KBank((address & 0xF8) >> 3);
}
