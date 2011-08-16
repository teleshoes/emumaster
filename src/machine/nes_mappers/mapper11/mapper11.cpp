#include "mapper11.h"

CpuMapper11::CpuMapper11(NesMapper *mapper) :
	NesCpuMapper(mapper) {
}

void CpuMapper11::reset() {
	setRom32KBank(0);
	if (mapper()->ppuMapper()->vromSize1KB())
		mapper()->ppuMapper()->setVrom8KBank(0);
	mapper()->ppuMapper()->setMirroring(NesPpuMapper::Vertical);
}

void CpuMapper11::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(address)
	setRom32KBank(data);
	NesPpuMapper *ppuMapper = mapper()->ppuMapper();
	if (ppuMapper->vromSize1KB())
		ppuMapper->setVrom8KBank(data >> 4);
}

NES_MAPPER_PLUGIN_EXPORT(11, "Color Dreams")
