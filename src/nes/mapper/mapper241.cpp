#include "mapper241.h"
#include "ppu.h"
#include "disk.h"

void Mapper241::reset() {
	NesMapper::reset();
	setRom32KBank(0);
	if (nesVromSize1KB)
		setVrom8KBank(0);
}

void Mapper241::writeHigh(u16 address, u8 data) {
	if (address == 0x8000)
		setRom32KBank(data);
}
