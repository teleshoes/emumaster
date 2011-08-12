#include "mapper229.h"

CpuMapper229::CpuMapper229(NesMapper *mapper) :
	NesCpuMemoryMapper(mapper) {
}

void CpuMapper229::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(data)
	if (address & 0x1E) {
		setRom16KBank(0, address & 0x1F);
		setRom16KBank(1, address & 0x1F);
		mapper()->ppuMemory()->setRomBank(address & 0x0FFF);
	} else {
		setRomBank(0);
		mapper()->ppuMemory()->setRomBank(0);
	}
	if (address & 0x0020)
		mapper()->ppuMemory()->setMirroring(NesPpuMemoryMapper::Horizontal);
	else
		mapper()->ppuMemory()->setMirroring(NesPpuMemoryMapper::Vertical);
}

NES_MAPPER_PLUGIN_EXPORT(229, "31-in-1")
