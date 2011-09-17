#include "mapper240.h"
#include "ppu.h"
#include "disk.h"

void Mapper240::reset() {
	NesMapper::reset();

	setRom8KBanks(0, 1, nesRomSize8KB-2, nesRomSize8KB-1);
	if (nesVromSize1KB)
		setVrom8KBank(0);
}

void Mapper240::writeLow(u16 address, u8 data) {
	if (address>=0x4020 && address < 0x6000) {
		setRom32KBank((data & 0xF0) >> 4);
		setVrom8KBank(data & 0xF);
	}
}
