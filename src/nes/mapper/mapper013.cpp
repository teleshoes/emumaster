#include "mapper013.h"

void Mapper013::reset() {
	NesMapper::reset();
	setRom32KBank(0);
	setCram4KBank(0, 0);
	setCram4KBank(4, 0);
}

void Mapper013::writeHigh(u16 address, u8 data) {
	Q_UNUSED(address)
	setRom32KBank((data & 0x30) >> 4);
	setCram4KBank(4, data & 0x03);
}
