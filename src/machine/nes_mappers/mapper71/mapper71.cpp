#include "mapper71.h"

CpuMapper71::CpuMapper71(NesMapper *mapper) :
	NesCpuMemoryMapper(mapper) {
	setRom16KBank(0, 0);
	setRom16KBank(1, romSize16KB() - 1);
}

void CpuMapper71::writeHigh(quint16 address, quint8 data) {
	switch(address & 0xF000) {
	case 0x9000:
		if(data & 0x10)
			mapper()->ppuMemory()->setMirroring(NesPpuMemoryMapper::SingleHigh);
		else
			mapper()->ppuMemory()->setMirroring(NesPpuMemoryMapper::SingleLow);
		break;
	case 0xC000:
	case 0xD000:
	case 0xE000:
	case 0xF000:
		setRom16KBank(0, data);
		break;
	default:
		break;
	}
}

void CpuMapper71::write(quint16 address, quint8 data) {
	if((address & 0xE000) == 0x6000)
		setRom16KBank(0, data);
	else
		NesCpuMemoryMapper::write(address, data);
}

NES_MAPPER_PLUGIN_SOURCE(71, "Camerica")
