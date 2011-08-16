#include "mapper61.h"

CpuMapper61::CpuMapper61(NesMapper *mapper) :
	NesCpuMapper(mapper) {
}

void CpuMapper61::reset() {
	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);
}

void CpuMapper61::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(data)
	switch (address & 0x30) {
	case 0x00:
	case 0x30:
		setRom32KBank(address & 0x0F);
		break;
	case 0x10:
	case 0x20:
		uint bank = ((address&0x0F)<<1) | ((address&0x20)>>4);
		setRom16KBank(4, bank);
		setRom16KBank(6, bank);
		break;
	}
	mapper()->ppuMapper()->setMirroring(static_cast<NesPpuMapper::Mirroring>((address & 0x80) >> 7));
}

NES_MAPPER_PLUGIN_EXPORT(61, "?")
