#include "mapper57.h"
#include <QDataStream>

CpuMapper57::CpuMapper57(NesMapper *mapper) :
	NesCpuMapper(mapper) {
}

void CpuMapper57::reset() {
	NesCpuMapper::reset();
	setRom16KBank(0, 0);
	setRom16KBank(1, 0);
	mapper()->ppuMemory()->setRomBank(0);
	m_reg = 0;
}

void CpuMapper57::writeHigh(quint16 address, quint8 data) {
	switch (address) {
	case 0x8000:
	case 0x8001:
	case 0x8002:
	case 0x8003:
		if (data & 0x40)
			mapper()->ppuMemory()->setRomBank((data&0x03) + ((m_reg&0x10)>>1) + (m_reg&0x07));
		break;
	case 0x8800:
		m_reg = data;
		if (data & 0x80) {
			setRomBank(((data & 0x40) >> 6) + 2);
		} else {
			setRom16KBank(0, (data & 0x60) >> 5);
			setRom16KBank(1, (data & 0x60) >> 5);
		}
		mapper()->ppuMemory()->setRomBank((data&0x07) + ((data&0x10)>>1));
		if (data & 0x08)
			mapper()->ppuMemory()->setMirroring(NesPpuMapper::Horizontal);
		else
			mapper()->ppuMemory()->setMirroring(NesPpuMapper::Vertical);
		break;
	}
}

void CpuMapper57::save(QDataStream &s) {
	NesCpuMapper::save(s);
	s << m_reg;
}

bool CpuMapper57::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	s >> m_reg;
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(57, "57")
