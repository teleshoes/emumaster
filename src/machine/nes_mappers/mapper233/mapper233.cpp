#include "mapper233.h"

CpuMapper233::CpuMapper233(NesMapper *mapper) :
	NesCpuMemoryMapper(mapper) {
}

void CpuMapper233::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(address)
	if (data & 0x20) {
		setRom16KBank(0, data & 0x1F);
		setRom16KBank(1, data & 0x1F);
	} else {
		setRomBank(((data & 0x1E) >> 1) * 4);
	}
	switch (data >> 6) {
	case 0: mapper()->ppuMemory()->setMirroring(0, 0, 0, 1);
	case 1: mapper()->ppuMemory()->setMirroring(NesPpuMemoryMapper::Vertical);
	case 2: mapper()->ppuMemory()->setMirroring(NesPpuMemoryMapper::Horizontal);
	case 3: mapper()->ppuMemory()->setMirroring(NesPpuMemoryMapper::SingleHigh);
	}
}

NES_MAPPER_PLUGIN_SOURCE(233, "42-in-1")
