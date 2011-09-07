#include "mapper240.h"
#include "nesppu.h"
#include "nesdisk.h"

void Mapper240::reset() {
	NesMapper::reset();

	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);
	if (vromSize1KB())
		setVrom8KBank(0);
}

void Mapper240::writeLow(quint16 address, quint8 data) {
	if (address>=0x4020 && address < 0x6000) {
		setRom32KBank((data & 0xF0) >> 4);
		setVrom8KBank(data & 0xF);
	}
}
