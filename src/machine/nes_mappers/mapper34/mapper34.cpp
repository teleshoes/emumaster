#include "mapper34.h"

CpuMapper34::CpuMapper34(NesMapper *mapper) :
	NesCpuMemoryMapper(mapper) {
}

void CpuMapper34::write(quint16 address, quint8 data) {
	if (address >= 0x7FFD) {
		if (address == 0x7FFD)
			setRomBank(data);
		else if (address == 0x7FFE)
			mapper()->ppuMemory()->setRom4KBank(0, data);
		else // 0x7FFF
			mapper()->ppuMemory()->setRom4KBank(1, data);
	} else {
		NesCpuMemoryMapper::write(address, data);
	}
}

void CpuMapper34::reset() {
	NesCpuMemoryMapper::reset();
	setRom16KBank(0, 0);
	setRom16KBank(1, romSize16KB()-1);
	if (mapper()->ppuMemory()->romSize())
		mapper()->ppuMemory()->setRomBank(0);
}

void CpuMapper34::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(address)
	setRomBank(data);
}

NES_MAPPER_PLUGIN_EXPORT(34, "Nina-1")
