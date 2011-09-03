#include "mapper66.h"

CpuMapper66::CpuMapper66(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper66::reset() {
	ppuMapper = mapper()->ppuMapper();

	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);
	ppuMapper->setVrom8KBank(0);
}

void CpuMapper66::writeLow(quint16 address, quint8 data) {
	if(address >= 0x6000) {
		setRom32KBank((data & 0xF0) >> 4);
		ppuMapper->setVrom8KBank(data & 0x0F);
	}
}

void CpuMapper66::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(address)
	setRom32KBank((data & 0xF0) >> 4);
	ppuMapper->setVrom8KBank(data & 0x0F);
}

NES_MAPPER_PLUGIN_EXPORT(66, "Bandai 74161")
