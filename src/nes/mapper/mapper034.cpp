#include "mapper034.h"

void Mapper034::reset() {
	NesMapper::reset();

	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);
	if (vromSize1KB())
		setVrom8KBank(0);
}

void Mapper034::writeLow(quint16 address, quint8 data) {
	if (address == 0x7FFD)
		setRom32KBank(data);
	else if (address == 0x7FFE)
		setVrom4KBank(0, data);
	else if (address == 0x7FFF)
		setVrom4KBank(4, data);
}

void Mapper034::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(address)
	setRom32KBank(data);
}