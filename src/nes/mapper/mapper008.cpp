#include "mapper008.h"

void Mapper008::reset() {
	NesMapper::reset();
	setRom32KBank(0);
	setVrom8KBank(0);
}

void Mapper008::writeHigh(u16 address, u8 data) {
	Q_UNUSED(address)
	setRom16KBank(4, (data & 0xF8) >> 3);
	setVrom8KBank(data & 0x07);
}
