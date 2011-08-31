#include "mapper232.h"
#include "nesppu.h"
#include "nesdisk.h"
#include <QDataStream>

CpuMapper232::CpuMapper232(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper232::reset() {
	ppuMapper = mapper()->ppuMapper();

	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);

	reg[0] = 0x0C;
	reg[1] = 0x00;
}

void CpuMapper232::writeLow(quint16 address, quint8 data) {
	if (address >= 0x6000)
		write(address, data);
}

void CpuMapper232::writeHigh(quint16 address, quint8 data) {
	if (address <= 0x9FFF)
		reg[0] = (data & 0x18)>>1;
	else
		reg[1] = data & 0x03;

	setRom8KBank(4, (reg[0]|reg[1])*2+0);
	setRom8KBank(5, (reg[0]|reg[1])*2+1);
	setRom8KBank(6, (reg[0]|0x03)*2+0);
	setRom8KBank(7, (reg[0]|0x03)*2+1);
}

bool CpuMapper232::save(QDataStream &s) {
	if (!NesCpuMapper::save(s))
		return false;
	for (int i = 0; i < 2; i++)
		s << reg[i];
	return true;
}

bool CpuMapper232::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	for (int i = 0; i < 2; i++)
		s >> reg[i];
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(232, "Quattro Games")
