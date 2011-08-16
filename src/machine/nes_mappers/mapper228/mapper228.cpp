#include "mapper228.h"

CpuMapper228::CpuMapper228(NesMapper *mapper) :
	NesCpuMapper(mapper) {
}

void CpuMapper228::writeHigh(quint16 address, quint8 data) {
	quint8 prg = (address & 0x0780) >> 7;
	switch ((address & 0x1800) >> 11) {
	case 1: prg |= 0x10; break;
	case 3: prg |= 0x20; break;
	default: break;
	}
	if (address & 0x0020) {
		prg <<= 1;
		if (address & 0x0040)
			prg++;
		setRom16KBank(0, prg * 2);
		setRom16KBank(1, prg * 2);
	} else {
		setRomBank(prg);
	}
	mapper()->ppuMemory()->setRomBank(((address & 0x000F) << 2) | (data & 0x03));
	if (address & 0x2000)
		mapper()->ppuMemory()->setMirroring(NesPpuMapper::Horizontal);
	else
		mapper()->ppuMemory()->setMirroring(NesPpuMapper::Vertical);
}

NES_MAPPER_PLUGIN_EXPORT(228, "Action 52")
