#include "mapper46.h"

CpuMapper46::CpuMapper46(NesMapper *mapper) :
	NesCpuMapper(mapper) {
}

void CpuMapper46::reset() {
	NesCpuMapper::reset();
	qMemSet(m_reg[0], 0, 4);
	updateBanks();
	mapper()->ppuMemory()->setMirroring(NesPpuMapper::Vertical);
}

void CpuMapper46::write(quint16 address, quint8 data) {
	if (address >= 0x4100) {
		m_reg[0] = data & 0x0F;
		m_reg[1] = (data & 0xF0) >> 4;
		updateBanks();
	} else {
		NesCpuMapper::write(address, data);
	}
}

void CpuMapper46::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(address)
	m_reg[2] = data & 0x01;
	m_reg[3] = (data & 0x70) >> 4;
	updateBanks();
}

void CpuMapper46::updateBanks() {
	setRomBank(m_reg[0]*2+m_reg[2]);
	mapper()->ppuMemory()->setRomBank(m_reg[1]*8+m_reg[3]);
}

void CpuMapper46::save(QDataStream &s) {
	NesCpuMapper::save(s);
	for (int i = 0; i < sizeof(m_reg); i++)
		s << m_reg[i];
}

bool CpuMapper46::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	for (int i = 0; i < sizeof(m_reg); i++)
		s >> m_reg[i];
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(46, "Rumble Station")
