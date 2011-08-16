#include "mapper51.h"
#include <QDataStream>

CpuMapper51::CpuMapper51(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper51::reset() {
	ppuMapper = mapper()->ppuMapper();

	bank = 0;
	mode = 1;
	updateBanks();
	ppuMapper->setCram8KBank(0);
}

void CpuMapper51::writeLow(quint16 address, quint8 data) {
	if (address >= 0x6000) {
		mode = ((data & 0x10) >> 3) | ((data & 0x02) >> 1);
		updateBanks();
	}
}

void CpuMapper51::writeHigh(quint16 address, quint8 data) {
	bank = (data & 0x0f) << 2;
	if (0xC000 <= address && address <= 0xDFFF)
		mode = (mode & 0x01) | ((data & 0x10) >> 3);
	updateBanks();
}

void CpuMapper51::updateBanks() {
	switch (mode) {
	case 0:
		ppuMapper->setMirroring(NesPpuMapper::Vertical);
		setRom8KBank(3, (bank|0x2c|3));
		setRom8KBank(4, (bank|0x00|0));
		setRom8KBank(5, (bank|0x00|1));
		setRom8KBank(6, (bank|0x0c|2));
		setRom8KBank(7, (bank|0x0c|3));
		break;
	case 1:
		ppuMapper->setMirroring(NesPpuMapper::Vertical);
		setRom8KBank(3, (bank|0x20|3));
		setRom8KBank(4, (bank|0x00|0));
		setRom8KBank(5, (bank|0x00|1));
		setRom8KBank(6, (bank|0x00|2));
		setRom8KBank(7, (bank|0x00|3));
		break;
	case 2:
		ppuMapper->setMirroring(NesPpuMapper::Vertical);
		setRom8KBank(3, (bank|0x2e|3));
		setRom8KBank(4, (bank|0x02|0));
		setRom8KBank(5, (bank|0x02|1));
		setRom8KBank(6, (bank|0x0e|2));
		setRom8KBank(7, (bank|0x0e|3));
		break;
	case 3:
		ppuMapper->setMirroring(NesPpuMapper::Horizontal);
		setRom8KBank(3, (bank|0x20|3));
		setRom8KBank(4, (bank|0x00|0));
		setRom8KBank(5, (bank|0x00|1));
		setRom8KBank(6, (bank|0x00|2));
		setRom8KBank(7, (bank|0x00|3));
		break;
	}
}

bool CpuMapper51::save(QDataStream &s) {
	if (!NesCpuMapper::save(s))
		return false;
	s << mode;
	s << bank;
	return true;
}

bool CpuMapper51::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	s >> mode;
	s >> bank;
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(51, "11-in-1")
