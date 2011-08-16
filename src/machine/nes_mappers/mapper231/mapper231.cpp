#include "mapper231.h"

CpuMapper231::CpuMapper231(NesMapper *mapper) :
	NesCpuMapper(mapper) {
}

void CpuMapper231::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(data)
	if (address & 0x0020)
		setRomBank((address >> 1) & 0xFF);
	else {
		setRom16KBank(0, address & 0x1E);
		setRom16KBank(1, address & 0x1E);
	}
	if(address & 0x0080)
		mapper()->ppuMemory()->setMirroring(NesPpuMapper::Horizontal);
	else
		mapper()->ppuMemory()->setMirroring(NesPpuMapper::Vertical);
}

NES_MAPPER_PLUGIN_EXPORT(231, "20-in-1")
