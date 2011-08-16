#include "mapper22.h"

CpuMapper22::CpuMapper22(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper22::reset() {
	ppuMapper = mapper()->ppuMapper();
	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);
}

void CpuMapper22::writeHigh(quint16 address, quint8 data) {
	switch (address) {
	case 0x8000:
		setRom8KBank(4, data);
		break;
	case 0x9000:
		ppuMapper->setMirroring(static_cast<NesPpuMapper::Mirroring>(data & 3));
		break;
	case 0xA000: setRom8KBank(5, data); break;
	case 0xB000: ppuMapper->setVrom1KBank(0, data >> 1); break;
	case 0xB001: ppuMapper->setVrom1KBank(1, data >> 1); break;
	case 0xC000: ppuMapper->setVrom1KBank(2, data >> 1); break;
	case 0xC001: ppuMapper->setVrom1KBank(3, data >> 1); break;
	case 0xD000: ppuMapper->setVrom1KBank(4, data >> 1); break;
	case 0xD001: ppuMapper->setVrom1KBank(5, data >> 1); break;
	case 0xE000: ppuMapper->setVrom1KBank(6, data >> 1); break;
	case 0xE001: ppuMapper->setVrom1KBank(7, data >> 1); break;
	}
}

NES_MAPPER_PLUGIN_EXPORT(22, "Konami VRC2 type A")
