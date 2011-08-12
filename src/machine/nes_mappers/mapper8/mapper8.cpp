#include "mapper8.h"

CpuMapper8::CpuMapper8(NesMapper *mapper) :
	NesCpuMemoryMapper(mapper) {
}

void CpuMapper8::reset() {
	NesCpuMemoryMapper::reset();
	setRomBank(0);
	mapper()->ppuMemory()->setRomBank(0);
}

void CpuMapper8::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(address)
	setRom16KBank(0, (data & 0xF8) >> 3);
	mapper()->ppuMemory()->setRomBank(data & 0x07);
}

NES_MAPPER_PLUGIN_EXPORT(8, "FFE F3xxx")
