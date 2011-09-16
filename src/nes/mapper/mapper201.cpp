#include "mapper201.h"
#include "ppu.h"
#include "disk.h"

void Mapper201::reset() {
	NesMapper::reset();

	setRom16KBank(4, 0);
	setRom16KBank(6, 0);
	if (vromSize1KB())
		setVrom8KBank(0);
}

void Mapper201::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(data)

	quint8 bank = address & 0x03;
	if (!(address & 0x08))
		bank = 0;
	setRom32KBank(bank);
	setVrom8KBank(bank);
}
