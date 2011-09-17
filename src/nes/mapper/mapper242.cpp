#include "mapper242.h"
#include "ppu.h"
#include "disk.h"

void Mapper242::reset() {
	NesMapper::reset();
	setRom32KBank(0);
}

void Mapper242::writeHigh(u16 address, u8 data) {
	Q_UNUSED(data)
	if (address & 0x01)
		setRom32KBank((address & 0xF8) >> 3);
}
