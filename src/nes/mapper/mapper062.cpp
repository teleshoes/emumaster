#include "mapper062.h"

void Mapper062::reset() {
	NesMapper::reset();

	setRom32KBank(0);
	setVrom8KBank(0);
}

void Mapper062::writeHigh(quint16 address, quint8 data) {
	switch (address & 0xFF00) {
	case 0x8100:
		setRom8KBank(4, data);
		setRom8KBank(5, data+1);
		break;
	case 0x8500:
		setRom8KBank(4, data);
		break;
	case 0x8700:
		setRom8KBank(5, data);
		break;
	}
	for (int i = 0; i < 8; i++)
		setVrom1KBank(i, data+i);
}
