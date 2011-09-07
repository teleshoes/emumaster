#include "mapper058.h"

void Mapper058::reset() {
	NesMapper::reset();

	setRom8KBanks(0, 1, 0, 1);
	if (vromSize1KB())
		setVrom8KBank(0);
}

void Mapper058::writeHigh(quint16 address, quint8 data) {
	if (address & 0x40) {
		setRom16KBank(4, address & 0x07);
		setRom16KBank(6, address & 0x07);
	} else {
		setRom32KBank((address & 0x06) >> 1);
	}
	if (vromSize1KB())
		setVrom8KBank((address & 0x38) >> 3);
	setMirroring(static_cast<Mirroring>((data & 0x02) >> 1));
}
