#include "mapper8.h"

CpuMapper8::CpuMapper8(NesMapper *mapper) :
	NesCpuMapper(mapper) {
}

void CpuMapper8::reset() {
	setRom32KBank(0);
	mapper()->ppuMapper()->setVrom8KBank(0);
}

void CpuMapper8::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(address)
	setRom16KBank(4, (data & 0xF8) >> 3);
	mapper()->ppuMapper()->setVrom8KBank(data & 0x07);
}

NES_MAPPER_PLUGIN_EXPORT(8, "FFE F3xxx")
