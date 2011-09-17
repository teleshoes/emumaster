#include "mapper066.h"

void Mapper066::reset() {
	NesMapper::reset();

	setRom8KBanks(0, 1, nesRomSize8KB-2, nesRomSize8KB-1);
	setVrom8KBank(0);
}

void Mapper066::writeLow(u16 address, u8 data) {
	if(address >= 0x6000) {
		setRom32KBank((data & 0xF0) >> 4);
		setVrom8KBank(data & 0x0F);
	}
}

void Mapper066::writeHigh(u16 address, u8 data) {
	Q_UNUSED(address)
	setRom32KBank((data & 0xF0) >> 4);
	setVrom8KBank(data & 0x0F);
}
