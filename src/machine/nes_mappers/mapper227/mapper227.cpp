#include "mapper227.h"

CpuMapper227::CpuMapper227(NesMapper *mapper) :
	NesCpuMemoryMapper(mapper) {
	setRom16KBank(0, 0);
	setRom16KBank(1, 0);
}

void CpuMapper227::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(data)
	uint bank = (address >> 4) & 0x10;
	bank |= (address >> 3) & 0xF;
	if (address & 1) {
		setRomBank(bank);
	} else {
		if (address & 4) {
			setRom16KBank(0, bank * 2 + 1);
			setRom16KBank(1, bank * 2 + 1);
		} else {
			setRom16KBank(0, bank * 2);
			setRom16KBank(1, bank * 2);
		}
	}
	if (!(address & 0x0080)) {
		if (address & 0x0200)
			setRom16KBank(1, (bank & 0x1C) * 2 + 7);
		else
			setRom16KBank(1, (bank & 0x1C) * 2 + 0);
	}
	if (address & 2)
		mapper()->ppuMemory()->setMirroring(NesPpuMemoryMapper::Horizontal);
	else
		mapper()->ppuMemory()->setMirroring(NesPpuMemoryMapper::Vertical);
}

NES_MAPPER_PLUGIN_EXPORT(227, "1200-in-1")
