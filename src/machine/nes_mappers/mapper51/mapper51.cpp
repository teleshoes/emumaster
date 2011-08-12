#include "mapper51.h"
#include <QDataStream>

CpuMapper51::CpuMapper51(NesMapper *mapper) :
	NesCpuMemoryMapper(mapper),
	m_lowMemBankData(0) {
}

void CpuMapper51::reset() {
	NesCpuMemoryMapper::reset();
	bank = 0;
	mode = 1;
	updateBanks();
	mapper()->ppuMemory()->setCramBank(0);
}

quint8 CpuMapper51::read(quint16 address) {
	if (address >= 0x6000 && address < 0x8000)
		return m_lowMemBankData[address - 0x6000];
	return NesCpuMemoryMapper::read(address);
}

void CpuMapper51::write(quint16 address, quint8 data) {
	if (address >= 0x4100) {
		if (address >= 0x6000) {
			mode = ((data & 0x10) >> 3) | ((data & 0x02) >> 1);
			updateBanks();
		}
	} else {
		NesCpuMemoryMapper::write(address, data);
	}
}

void CpuMapper51::writeHigh(quint16 address, quint8 data) {
	bank = (data & 0x0f) << 2;
	if (address >= 0xC000 && address < 0xE000)
		mode = (mode & 0x01) | ((data & 0x10) >> 3);
	updateBanks();
}

void CpuMapper51::updateBanks() {
	NesPpuMemoryMapper *ppuMapper = mapper()->ppuMemory();
	switch (mode) {
	case 0:
		ppuMapper->setMirroring(NesPpuMemoryMapper::Vertical);
		m_lowMemBankData = rom8KBankData(bank|0x2c|3);
		setRom8KBank(0, (bank|0x00|0));
		setRom8KBank(1, (bank|0x00|1));
		setRom8KBank(2, (bank|0x0c|2));
		setRom8KBank(3, (bank|0x0c|3));
		break;
	case 1:
		ppuMapper->setMirroring(NesPpuMemoryMapper::Vertical);
		m_lowMemBankData = rom8KBankData(bank|0x20|3);
		setRom8KBank(0, (bank|0x00|0));
		setRom8KBank(1, (bank|0x00|1));
		setRom8KBank(2, (bank|0x00|2));
		setRom8KBank(3, (bank|0x00|3));
		break;
	case 2:
		ppuMapper->setMirroring(NesPpuMemoryMapper::Vertical);
		m_lowMemBankData = rom8KBankData(bank|0x2e|3);
		setRom8KBank(0, (bank|0x02|0));
		setRom8KBank(1, (bank|0x02|1));
		setRom8KBank(2, (bank|0x0e|2));
		setRom8KBank(3, (bank|0x0e|3));
		break;
	case 3:
		ppuMapper->setMirroring(NesPpuMemoryMapper::Horizontal);
		m_lowMemBankData = rom8KBankData(bank|0x20|3);
		setRom8KBank(0, (bank|0x00|0));
		setRom8KBank(1, (bank|0x00|1));
		setRom8KBank(2, (bank|0x00|2));
		setRom8KBank(3, (bank|0x00|3));
		break;
	default:
		Q_ASSERT(false);
	}
}

void CpuMapper51::save(QDataStream &s) {
	NesCpuMemoryMapper::save(s);
	s << mode;
	s << bank;
}

bool CpuMapper51::load(QDataStream &s) {
	if (!NesCpuMemoryMapper::load(s))
		return false;
	s >> mode;
	s >> bank;
	updateBanks();
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(51, "11-in-1")
