#include "mapper251.h"
#include "nesppu.h"
#include "nesdisk.h"
#include <QDataStream>

CpuMapper251::CpuMapper251(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper251::reset() {
	ppuMapper = mapper()->ppuMapper();

	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);
	ppuMapper->setMirroring(NesPpuMapper::Vertical);

	qMemSet(reg, 0, sizeof(reg));
	qMemSet(breg, 0, sizeof(breg));
}

void CpuMapper251::writeLow(quint16 address, quint8 data) {
	Q_UNUSED(address)
	if ((address & 0xE001) == 0x6000) {
		if (reg[9]) {
			breg[reg[10]++] = data;
			if (reg[10] == 4) {
				reg[10] = 0;
				setBank();
			}
		}
	}
}

void CpuMapper251::writeHigh(quint16 address, quint8 data) {
	switch (address & 0xE001) {
	case 0x8000:
		reg[8] = data;
		setBank();
		break;
	case 0x8001:
		reg[reg[8]&0x07] = data;
		setBank();
		break;
	case 0xA001:
		if (data & 0x80) {
			reg[ 9] = 1;
			reg[10] = 0;
		} else {
			reg[ 9] = 0;
		}
		break;
	}
}

void CpuMapper251::setBank() {
	int chr[6];
	int prg[4];

	for (int i = 0; i < 6; i++)
		chr[i] = (reg[i]|(breg[1]<<4)) & ((breg[2]<<4)|0x0F);

	if (reg[8] & 0x80) {
		ppuMapper->setVrom1KBank(4, chr[0]);
		ppuMapper->setVrom1KBank(5, chr[0]+1);
		ppuMapper->setVrom1KBank(6, chr[1]);
		ppuMapper->setVrom1KBank(7, chr[1]+1);
		ppuMapper->setVrom1KBank(0, chr[2]);
		ppuMapper->setVrom1KBank(1, chr[3]);
		ppuMapper->setVrom1KBank(2, chr[4]);
		ppuMapper->setVrom1KBank(3, chr[5]);
	} else {
		ppuMapper->setVrom1KBank(0, chr[0]);
		ppuMapper->setVrom1KBank(1, chr[0]+1);
		ppuMapper->setVrom1KBank(2, chr[1]);
		ppuMapper->setVrom1KBank(3, chr[1]+1);
		ppuMapper->setVrom1KBank(4, chr[2]);
		ppuMapper->setVrom1KBank(5, chr[3]);
		ppuMapper->setVrom1KBank(6, chr[4]);
		ppuMapper->setVrom1KBank(7, chr[5]);
	}

	prg[0] = (reg[6]&((breg[3]&0x3F)^0x3F))|(breg[1]);
	prg[1] = (reg[7]&((breg[3]&0x3F)^0x3F))|(breg[1]);
	prg[2] = prg[3] =((breg[3]&0x3F)^0x3F)|(breg[1]);
	prg[2] &= romSize8KB()-1;

	if (reg[8] & 0x40)
		setRom8KBanks(prg[2],prg[1],prg[0],prg[3]);
	else
		setRom8KBanks(prg[0],prg[1],prg[2],prg[3]);
}

bool CpuMapper251::save(QDataStream &s) {
	if (!NesCpuMapper::save(s))
		return false;
	for (int i = 0; i < 11; i++)
		s << reg[i];
	for (int i = 0; i < 4; i++)
		s << breg[i];
	return true;
}

bool CpuMapper251::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	for (int i = 0; i < 11; i++)
		s >> reg[i];
	for (int i = 0; i < 4; i++)
		s >> breg[i];
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(251, "-")
