#include "mapper13.h"

CpuMapper13::CpuMapper13(NesMapper *mapper) :
	NesCpuMapper(mapper) {
}

void CpuMapper13::reset() {
	setRom32KBank(0);
	mapper()->ppuMapper()->setCram4KBank(0, 0);
	mapper()->ppuMapper()->setCram4KBank(4, 0);
}

void CpuMapper13::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(address)
	setRom32KBank((data & 0x30) >> 4);
	mapper()->ppuMapper()->setCram4KBank(4, data & 0x03);
}

NES_MAPPER_PLUGIN_EXPORT(13, "CPROM")
