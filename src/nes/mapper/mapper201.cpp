#include "mapper201.h"
#include "ppu.h"
#include "disk.h"

void Mapper201::reset() {
	NesMapper::reset();

	setRom16KBank(4, 0);
	setRom16KBank(6, 0);
	if (nesVromSize1KB)
		setVrom8KBank(0);
}

void Mapper201::writeHigh(u16 address, u8 data) {
	Q_UNUSED(data)

	u8 bank = address & 0x03;
	if (!(address & 0x08))
		bank = 0;
	setRom32KBank(bank);
	setVrom8KBank(bank);
}
