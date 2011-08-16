#include "mapper41.h"
#include <QDataStream>

CpuMapper41::CpuMapper41(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper41::reset() {
	ppuMapper = mapper()->ppuMapper();

	reg[0] = reg[1] = 0;
	setRom32KBank(0);
	if (ppuMapper->vromSize1KB())
		ppuMapper->setVrom8KBank(0);
}

void CpuMapper41::writeLow(quint16 address, quint8 data) {
	Q_UNUSED(data)
	if (address >= 0x6000 && address < 0x6800) {
		setRom32KBank(address & 0x07);
		reg[0] = address & 0x04;
		reg[1] &= 0x03;
		reg[1] |= (address>>1) & 0x0C;
		ppuMapper->setVrom8KBank(reg[1]);
		ppuMapper->setMirroring(static_cast<NesPpuMapper::Mirroring>((data & 0x20) >> 5));
	}
}

void CpuMapper41::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(data)
	if (reg[0]) {
		reg[1] &= 0x0C;
		reg[1] &= address & 0x03;
		ppuMapper->setVrom8KBank(reg[1]);
	}
}

bool CpuMapper41::save(QDataStream &s) {
	if (!NesCpuMapper::save(s))
		return false;
	s << reg[0] << reg[1];
	return true;
}

bool CpuMapper41::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	s >> reg[0] >> reg[1];
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(41, "Caltron 6-in-1")
