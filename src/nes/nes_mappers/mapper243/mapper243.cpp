#include "mapper243.h"
#include "nesppu.h"
#include "nesdisk.h"
#include <QDataStream>

CpuMapper243::CpuMapper243(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper243::reset() {
	ppuMapper = mapper()->ppuMapper();

	setRom32KBank(0);

	if (ppuMapper->vromSize8KB() > 4)
		ppuMapper->setVrom8KBank(4);
	else
		ppuMapper->setVrom8KBank(0);

	ppuMapper->setMirroring(NesPpuMapper::Vertical);

	for (int i = 0; i < 4; i++)
		reg[i] = 0;
}

void CpuMapper243::writeLow(quint16 address, quint8 data) {
	if ((address&0x4101) == 0x4100) {
		reg[0] = data;
	} else if ((address&0x4101) == 0x4101) {
		switch (reg[0] & 0x07) {
		case 0:
			reg[1] = 0;
			reg[2] = 3;
			break;
		case 4:
			reg[2] = (reg[2]&0x06)|(data&0x01);
			break;
		case 5:
			reg[1] = data&0x01;
			break;
		case 6:
			reg[2] = (reg[2]&0x01)|((data&0x03)<<1);
			break;
		case 7:
			reg[3] = data&0x01;
			break;
		default:
			break;
		}

		setRom32KBank(reg[1]);
		ppuMapper->setVrom8KBank(reg[2]);

		if (reg[3])
			ppuMapper->setMirroring(NesPpuMapper::Vertical);
		else
			ppuMapper->setMirroring(NesPpuMapper::Horizontal);
	}
}

bool CpuMapper243::save(QDataStream &s) {
	if (!NesCpuMapper::save(s))
		return false;
	for (int i = 0; i < 4; i++)
		s << reg[i];
	return true;
}

bool CpuMapper243::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	for (int i = 0; i < 4; i++)
		s >> reg[i];
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(243, "PC-Sachen/Hacker")
