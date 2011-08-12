#include "mapper41.h"
#include <QDataStream>

CpuMapper41::CpuMapper41(NesMapper *mapper) :
	NesCpuMemoryMapper(mapper) {
}

void CpuMapper41::reset() {
	NesCpuMemoryMapper::reset();
	reg[0] = reg[1] = 0;
	setRomBank(0);
	if (mapper()->ppuMemory()->romSize())
		mapper()->ppuMemory()->setRomBank(0);
}

void CpuMapper41::write(quint16 address, quint8 data) {
	if( addr >= 0x6000 && addr < 0x6800) {
		setRomBank(address & 0x07);
		reg[0] = address & 0x04;
		reg[1] &= 0x03;
		reg[1] |= (address >> 1) & 0x0C;
		mapper()->ppuMemory()->setRomBank(reg[1]);
		if (address & 0x20)
			mapper()->ppuMemory()->setMirroring(NesPpuMemoryMapper::Horizontal);
		else
			mapper()->ppuMemory()->setMirroring(NesPpuMemoryMapper::Vertical);
	} else {
		NesCpuMemoryMapper::write(address, data);
	}
}

void CpuMapper41::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(address)
	if (reg[0]) {
		reg[1] &= 0x0C;
		reg[1] &= address & 0x03;
		mapper()->ppuMemory()->setRomBank(reg[1]);
	}
}

void CpuMapper41::save(QDataStream &s) {
	NesPpuMemoryMapper::save(s);
	s << reg[0] << reg[1];
}

bool CpuMapper41::load(QDataStream &s) {
	if (!NesPpuMemoryMapper::load(s))
		return false;
	s >> reg[0] >> reg[1];
}

NES_MAPPER_PLUGIN_EXPORT(41, "Caltron 6-in-1")
