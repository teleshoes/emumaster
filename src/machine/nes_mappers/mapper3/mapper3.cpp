#include "mapper3.h"

CpuMapper3::CpuMapper3(NesMapper *mapper) :
	NesCpuMemoryMapper(mapper) {
}

void CpuMapper3::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(address)
	mapper()->ppuMemory()->setRomBank(data);
}

NES_MAPPER_PLUGIN_SOURCE(3, "CNROM")
