#include "mapper57.h"
#include <QDataStream>

CpuMapper57::CpuMapper57(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper57::reset() {
	ppuMapper = mapper()->ppuMapper();

	setRom8KBanks(0, 1, 0, 1);
	ppuMapper->setVrom8KBank(0);

	reg = 0;
}

void CpuMapper57::writeHigh(quint16 address, quint8 data) {
	switch (address) {
	case 0x8000:
	case 0x8001:
	case 0x8002:
	case 0x8003:
		if (data & 0x40)
			ppuMapper->setVrom8KBank((data&0x03) + ((reg&0x10)>>1) + (reg&0x07));
		break;
	case 0x8800:
		reg = data;
		if (data & 0x80) {
			setRom32KBank(((data & 0x40) >> 6) + 2);
		} else {
			setRom16KBank(4, (data & 0x60) >> 5);
			setRom16KBank(6, (data & 0x60) >> 5);
		}
		ppuMapper->setVrom8KBank((data&0x07) + ((data&0x10)>>1));
		ppuMapper->setMirroring(static_cast<NesPpuMapper::Mirroring>((data & 0x08) >> 3));
		break;
	}
}

bool CpuMapper57::save(QDataStream &s) {
	if (!NesCpuMapper::save(s))
		return false;
	s << reg;
	return true;
}

bool CpuMapper57::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	s >> reg;
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(57, "57")
