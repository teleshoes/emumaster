#include "mapper58.h"

CpuMapper58::CpuMapper58(NesMapper *mapper) :
	NesCpuMemoryMapper(mapper) {
}

void CpuMapper58::reset() {
	NesCpuMemoryMapper::reset();
	setRom16KBank(0, 0);
	setRom16KBank(1, 0);
	if (mapper()->ppuMemory()->romSize())
		mapper()->ppuMemory()->setRomBank(0);
}

void CpuMapper58::writeHigh(quint16 address, quint8 data) {
	if (address & 0x40) {
		setRom16KBank(0, address & 0x07);
		setRom16KBank(1, address & 0x07);
	} else {
		setRomBank((address & 0x06) >> 1);
	}
	if (mapper()->ppuMemory()->romSize())
		mapper()->ppuMemory()->setRomBank((address & 0x38) >> 3);
	if (data & 0x02)
		mapper()->ppuMemory()->setMirroring(NesPpuMemoryMapper::Vertical);
	else
		mapper()->ppuMemory()->setMirroring(NesPpuMemoryMapper::Horizontal);
}

NES_MAPPER_PLUGIN_EXPORT(58, "58")
