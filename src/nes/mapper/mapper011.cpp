#include "mapper011.h"

void Mapper011::reset() {
	NesMapper::reset();
	setRom32KBank(0);
	if (nesVromSize1KB)
		setVrom8KBank(0);
	setMirroring(VerticalMirroring);
}

void Mapper011::writeHigh(u16 address, u8 data) {
	Q_UNUSED(address)
	setRom32KBank(data);
	if (nesVromSize1KB)
		setVrom8KBank(data >> 4);
}
