#include "mapper34.h"

CpuMapper34::CpuMapper34(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper34::reset() {
	ppuMapper = mapper()->ppuMapper();

	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);
	if (ppuMapper->vromSize1KB())
		ppuMapper->setVrom8KBank(0);
}

void CpuMapper34::writeLow(quint16 address, quint8 data) {
	if (address == 0x7FFD)
		setRom32KBank(data);
	else if (address == 0x7FFE)
		ppuMapper->setVrom4KBank(0, data);
	else if (address == 0x7FFF)
		ppuMapper->setVrom4KBank(4, data);
}

void CpuMapper34::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(address)
	setRom32KBank(data);
}

NES_MAPPER_PLUGIN_EXPORT(34, "Nina-1")
