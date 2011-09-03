#include "mapper255.h"
#include "nesppu.h"
#include "nesdisk.h"
#include <QDataStream>

CpuMapper255::CpuMapper255(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper255::reset() {
	ppuMapper = mapper()->ppuMapper();

	setRom32KBank(0);
	ppuMapper->setVrom8KBank(0);
	ppuMapper->setMirroring(NesPpuMapper::Vertical);

	for (int i = 0; i < 4; i++)
		reg[i] = 0;
}

quint8 CpuMapper255::readLow(quint16 address) {
	if (address >= 0x5800)
		return reg[address & 0x0003] & 0x0F;
	else
		return address >> 8;
}

void CpuMapper255::writeLow(quint16 address, quint8 data) {
	if (address >= 0x5800)
		reg[address & 0x0003] = data & 0x0F;
}

void CpuMapper255::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(data)

	quint8 prg = (address & 0x0F80) >> 7;
	int chr = (address & 0x003F);
	int bank = (address & 0x4000) >> 14;

	if (address & 0x2000)
		ppuMapper->setMirroring(NesPpuMapper::Horizontal);
	else
		ppuMapper->setMirroring(NesPpuMapper::Vertical);

	if (address & 0x1000) {
		if (address & 0x0040) {
			setRom8KBank(4, 0x80*bank+prg*4+2);
			setRom8KBank(5, 0x80*bank+prg*4+3);
			setRom8KBank(6, 0x80*bank+prg*4+2);
			setRom8KBank(7, 0x80*bank+prg*4+3);
		} else {
			setRom8KBank(4, 0x80*bank+prg*4+0);
			setRom8KBank(5, 0x80*bank+prg*4+1);
			setRom8KBank(6, 0x80*bank+prg*4+0);
			setRom8KBank(7, 0x80*bank+prg*4+1);
		}
	} else {
		setRom8KBank(4, 0x80*bank+prg*4+0);
		setRom8KBank(5, 0x80*bank+prg*4+1);
		setRom8KBank(6, 0x80*bank+prg*4+2);
		setRom8KBank(7, 0x80*bank+prg*4+3);
	}

	ppuMapper->setVrom1KBank(0, 0x200*bank+chr*8+0);
	ppuMapper->setVrom1KBank(1, 0x200*bank+chr*8+1);
	ppuMapper->setVrom1KBank(2, 0x200*bank+chr*8+2);
	ppuMapper->setVrom1KBank(3, 0x200*bank+chr*8+3);
	ppuMapper->setVrom1KBank(4, 0x200*bank+chr*8+4);
	ppuMapper->setVrom1KBank(5, 0x200*bank+chr*8+5);
	ppuMapper->setVrom1KBank(6, 0x200*bank+chr*8+6);
	ppuMapper->setVrom1KBank(7, 0x200*bank+chr*8+7);
}

bool CpuMapper255::save(QDataStream &s) {
	if (!NesCpuMapper::save(s))
		return false;
	for (int i = 0; i < 4; i++)
		s << reg[i];
	return true;
}

bool CpuMapper255::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	for (int i = 0; i < 4; i++)
		s >> reg[i];
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(255, "110-in-1")
