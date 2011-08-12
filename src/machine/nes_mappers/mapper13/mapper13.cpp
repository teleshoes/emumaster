#include "mapper13.h"

CpuMapper13::CpuMapper13(NesMapper *mapper) :
	NesCpuMemoryMapper(mapper) {
}

void CpuMapper13::reset() {
	NesCpuMemoryMapper::reset();
	setRomBank(0);
	mapper()->ppuMemory()->setCram4KBank(0, 0);
	mapper()->ppuMemory()->setCram4KBank(1, 0);
}

void CpuMapper13::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(address)
	setRomBank((data & 0x30) >> 4);
	mapper()->ppuMemory()->setCram4KBank(1, data & 0x03);
}

NES_MAPPER_PLUGIN_EXPORT(13, "CPROM")
