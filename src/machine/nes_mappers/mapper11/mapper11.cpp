#include "mapper11.h"

CpuMapper11::CpuMapper11(NesMapper *mapper) :
	NesCpuMemoryMapper(mapper) {
}

void CpuMapper11::reset() {
	NesCpuMemoryMapper::reset();
	setRomBank(0);
	if (mapper()->ppuMemory()->romSize())
		mapper()->ppuMemory()->setRomBank(0);
	mapper()->ppuMemory()->setMirroring(NesPpuMemoryMapper::Vertical);
}

void CpuMapper11::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(address)
	setRomBank(data);
	NesPpuMemoryMapper *ppuMapper = mapper()->ppuMemory();
	if (ppuMapper->romSize())
		ppuMapper->setRomBank(data >> 4);
}

NES_MAPPER_PLUGIN_EXPORT(11, "Color Dreams")
