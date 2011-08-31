#include "mapper222.h"
#include "nesppu.h"
#include "nesdisk.h"
#include <QDataStream>

CpuMapper222::CpuMapper222(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper222::reset() {
	ppuMapper = mapper()->ppuMapper();

	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);
	if (ppuMapper->vromSize1KB())
		ppuMapper->setVrom8KBank(0);
	ppuMapper->setMirroring(NesPpuMapper::Vertical);
}

void CpuMapper222::writeHigh(quint16 address, quint8 data) {
	switch (address & 0xF003) {
	case 0x8000:
		setRom8KBank(4, data);
		break;
	case 0xA000:
		setRom8KBank(5, data);
		break;
	case 0xB000:
		ppuMapper->setVrom1KBank(0, data);
		break;
	case 0xB002:
		ppuMapper->setVrom1KBank(1, data);
		break;
	case 0xC000:
		ppuMapper->setVrom1KBank(2, data);
		break;
	case 0xC002:
		ppuMapper->setVrom1KBank(3, data);
		break;
	case 0xD000:
		ppuMapper->setVrom1KBank(4, data);
		break;
	case 0xD002:
		ppuMapper->setVrom1KBank(5, data);
		break;
	case 0xE000:
		ppuMapper->setVrom1KBank(6, data);
		break;
	case 0xE002:
		ppuMapper->setVrom1KBank(7, data);
		break;
	}
}

NES_MAPPER_PLUGIN_EXPORT(222, "-")
