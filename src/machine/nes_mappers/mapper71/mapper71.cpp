#include "mapper71.h"

CpuMapper71::CpuMapper71(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper71::reset() {
	ppuMapper = mapper()->ppuMapper();

	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);
}

void CpuMapper71::writeLow(quint16 address, quint8 data) {
	if ((address & 0xE000) == 0x6000)
		setRom16KBank(4, data);
}

void CpuMapper71::writeHigh(quint16 address, quint8 data) {
	switch (address & 0xF000) {
	case 0x9000:
		if (data & 0x10)
			ppuMapper->setMirroring(NesPpuMapper::SingleHigh);
		else
			ppuMapper->setMirroring(NesPpuMapper::SingleLow);
		break;
	case 0xC000:
	case 0xD000:
	case 0xE000:
	case 0xF000:
		setRom16KBank(4, data);
		break;
	default:
		break;
	}
}

NES_MAPPER_PLUGIN_EXPORT(71, "Camerica")
