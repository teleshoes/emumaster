#include "mapper071.h"

void Mapper071::reset() {
	NesMapper::reset();

	setRom8KBanks(0, 1, nesRomSize8KB-2, nesRomSize8KB-1);
}

void Mapper071::writeLow(u16 address, u8 data) {
	if ((address & 0xE000) == 0x6000)
		setRom16KBank(4, data);
}

void Mapper071::writeHigh(u16 address, u8 data) {
	switch (address & 0xF000) {
	case 0x9000:
		if (data & 0x10)
			setMirroring(SingleHigh);
		else
			setMirroring(SingleLow);
		break;
	case 0xC000:
	case 0xD000:
	case 0xE000:
	case 0xF000:
		setRom16KBank(4, data);
		break;
	default:
		break;
	}
}
