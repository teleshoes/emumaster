#include "mapper62.h"

CpuMapper62::CpuMapper62(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper62::reset() {
	ppuMapper = mapper()->ppuMapper();

	setRom32KBank(0);
	ppuMapper->setVrom8KBank(0);
}

void CpuMapper62::writeHigh(quint16 address, quint8 data) {
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
		ppuMapper->setVrom1KBank(i, data+i);
}

NES_MAPPER_PLUGIN_EXPORT(62, "?")
