#include "mapper3.h"
#include "nesdisk.h"

CpuMapper3::CpuMapper3(NesMapper *mapper) :
	NesCpuMapper(mapper) {
}

void CpuMapper3::reset() {
	setRom32KBank(0);
}

void CpuMapper3::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(address)
	mapper()->ppuMapper()->setVrom8KBank(data);
}

NES_MAPPER_PLUGIN_EXPORT(3, "CNROM")
