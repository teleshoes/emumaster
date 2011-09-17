#include "mapper003.h"
#include "disk.h"

void Mapper003::reset() {
	NesMapper::reset();

	setRom32KBank(0);
}

void Mapper003::writeHigh(u16 address, u8 data) {
	Q_UNUSED(address)
	setVrom8KBank(data);
}
