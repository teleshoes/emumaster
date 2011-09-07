#include "mapper202.h"
#include "nesppu.h"
#include "nesdisk.h"

void Mapper202::reset() {
	NesMapper::reset();

	setRom16KBank(4, 6);
	setRom16KBank(6, 7);
	if (vromSize1KB())
		setVrom8KBank(0);
}

void Mapper202::writeEx(quint16 address, quint8 data) {
	if (address >= 0x4020)
		writeSub(address, data);
}

void Mapper202::writeLow(quint16 address, quint8 data) {
	writeSub(address, data);
}

void Mapper202::writeHigh(quint16 address, quint8 data) {
	writeSub(address, data);
}

void Mapper202::writeSub(quint16 address, quint8 data) {
	Q_UNUSED(data)

	int bank = (address>>1) & 0x07;
	setRom16KBank(4, bank);
	if ((address & 0x0C) == 0x0C)
		setRom16KBank(6, bank+1);
	else
		setRom16KBank(6, bank);
	setVrom8KBank(bank);

	if (address & 0x01)
		setMirroring(Horizontal);
	else
		setMirroring(Vertical);
}
