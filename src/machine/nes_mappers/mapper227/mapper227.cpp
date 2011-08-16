#include "mapper227.h"

CpuMapper227::CpuMapper227(NesMapper *mapper) :
	NesCpuMapper(mapper) {
}

void CpuMapper227::reset() {
	setRom16KBank(4, 0);
	setRom16KBank(6, 0);
}

void CpuMapper227::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(data)
	uint bank = (address >> 4) & 0x10;
	bank |= (address >> 3) & 0xF;
	if (address & 1) {
		setRom32KBank(bank);
	} else {
		if (address & 4) {
			setRom16KBank(4, bank * 2 + 1);
			setRom16KBank(6, bank * 2 + 1);
		} else {
			setRom16KBank(4, bank * 2);
			setRom16KBank(6, bank * 2);
		}
	}
	if (!(address & 0x0080)) {
		if (address & 0x0200)
			setRom16KBank(6, (bank & 0x1C) * 2 + 7);
		else
			setRom16KBank(6, (bank & 0x1C) * 2 + 0);
	}
	if (address & 2)
		mapper()->ppuMapper()->setMirroring(NesPpuMapper::Horizontal);
	else
		mapper()->ppuMapper()->setMirroring(NesPpuMapper::Vertical);
}

NES_MAPPER_PLUGIN_EXPORT(227, "1200-in-1")
