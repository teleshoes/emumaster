#include "mapper46.h"
#include <QDataStream>

CpuMapper46::CpuMapper46(NesMapper *mapper) :
	NesCpuMapper(mapper) {
}

void CpuMapper46::reset() {
	qMemSet(reg, 0, 4);
	updateBanks();
	mapper()->ppuMapper()->setMirroring(NesPpuMapper::Vertical);
}

void CpuMapper46::writeLow(quint16 address, quint8 data) {
	Q_UNUSED(address)
	reg[0] = data & 0x0F;
	reg[1] = (data & 0xF0) >> 4;
	updateBanks();
}

void CpuMapper46::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(address)
	reg[2] = data & 0x01;
	reg[3] = (data & 0x70) >> 4;
	updateBanks();
}

void CpuMapper46::updateBanks() {
	setRom32KBank(reg[0]*2+reg[2]);
	mapper()->ppuMapper()->setVrom8KBank(reg[1]*8+reg[3]);
}

bool CpuMapper46::save(QDataStream &s) {
	if (!NesCpuMapper::save(s))
		return false;
	for (int i = 0; i < 4; i++)
		s << reg[i];
	return true;
}

bool CpuMapper46::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	for (int i = 0; i < 4; i++)
		s >> reg[i];
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(46, "Rumble Station")
