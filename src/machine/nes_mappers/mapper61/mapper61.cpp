#include "mapper61.h"

CpuMapper61::CpuMapper61(NesMapper *mapper) :
	NesCpuMapper(mapper) {
}

void CpuMapper61::reset() {
	NesCpuMapper::reset();
	setRom16KBank(0, 0);
	setRom16KBank(1, romSize16KB()-1);
}

void CpuMapper61::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(data)
	switch (address & 0x30) {
	case 0x00:
	case 0x30:
		setRomBank(address & 0x0F);
		break;
	case 0x10:
	case 0x20:
		uint bank = ((address&0x0F)<<1) | ((address&0x20)>>4);
		setRom16KBank(0, bank);
		setRom16KBank(1, bank);
		break;
	}
	if (address & 0x80)
		mapper()->ppuMemory()->setMirroring(NesPpuMapper::Horizontal);
	else
		mapper()->ppuMemory()->setMirroring(NesPpuMapper::Vertical);
}

NES_MAPPER_PLUGIN_EXPORT(61, "61")
