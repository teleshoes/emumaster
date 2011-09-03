#include "mapper58.h"

CpuMapper58::CpuMapper58(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper58::reset() {
	ppuMapper = mapper()->ppuMapper();

	setRom8KBanks(0, 1, 0, 1);
	if (ppuMapper->vromSize1KB())
		ppuMapper->setVrom8KBank(0);
}

void CpuMapper58::writeHigh(quint16 address, quint8 data) {
	if (address & 0x40) {
		setRom16KBank(4, address & 0x07);
		setRom16KBank(6, address & 0x07);
	} else {
		setRom32KBank((address & 0x06) >> 1);
	}
	if (ppuMapper->vromSize1KB())
		ppuMapper->setVrom8KBank((address & 0x38) >> 3);
	ppuMapper->setMirroring(static_cast<NesPpuMapper::Mirroring>((data & 0x02) >> 1));
}

NES_MAPPER_PLUGIN_EXPORT(58, "?")
