#include "mapper236.h"
#include "nesppu.h"
#include "nesdisk.h"
#include <QDataStream>

CpuMapper236::CpuMapper236(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper236::reset() {
	ppuMapper = mapper()->ppuMapper();

	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);
	bank = mode = 0;
}

void CpuMapper236::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(data)

	if (address >= 0x8000 && address <= 0xBFFF) {
		bank = ((address&0x03)<<4)|(bank&0x07);
	} else {
		bank = (address&0x07)|(bank&0x30);
		mode = address & 0x30;
	}

	if (address & 0x20)
		ppuMapper->setMirroring(NesPpuMapper::Horizontal);
	else
		ppuMapper->setMirroring(NesPpuMapper::Vertical);

	switch (mode) {
	case 0x00:
		bank |= 0x08;
		setRom8KBank(4, bank*2+0);
		setRom8KBank(5, bank*2+1);
		setRom8KBank(6, (bank|0x07)*2+0);
		setRom8KBank(7, (bank|0x07)*2+1);
		break;
	case 0x10:
		bank |= 0x37;
		setRom8KBank(4, bank*2+0);
		setRom8KBank(5, bank*2+1);
		setRom8KBank(6, (bank|0x07)*2+0);
		setRom8KBank(7, (bank|0x07)*2+1);
		break;
	case 0x20:
		bank |= 0x08;
		setRom8KBank(4, (bank&0xFE)*2+0);
		setRom8KBank(5, (bank&0xFE)*2+1);
		setRom8KBank(6, (bank&0xFE)*2+2);
		setRom8KBank(7, (bank&0xFE)*2+3);
		break;
	case 0x30:
		bank |= 0x08;
		setRom8KBank(4, bank*2+0);
		setRom8KBank(5, bank*2+1);
		setRom8KBank(6, bank*2+0);
		setRom8KBank(7, bank*2+1);
		break;
	}
}

bool CpuMapper236::save(QDataStream &s) {
	if (!NesCpuMapper::save(s))
		return false;
	s << bank;
	s << mode;
	return true;
}

bool CpuMapper236::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	s >> bank;
	s >> mode;
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(236, "800-in-1")
