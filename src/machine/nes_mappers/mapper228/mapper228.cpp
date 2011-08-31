#include "mapper228.h"
#include "nesppu.h"
#include "nesdisk.h"
#include <QDataStream>

CpuMapper228::CpuMapper228(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper228::reset() {
	ppuMapper = mapper()->ppuMapper();

	setRom32KBank(0);
	ppuMapper->setVrom8KBank(0);
}

void CpuMapper228::writeHigh(quint16 address, quint8 data) {
	quint8 prg = (address&0x0780) >> 7;

	switch ((address&0x1800) >> 11) {
	case 1:
		prg |= 0x10;
		break;
	case 3:
		prg |= 0x20;
		break;
	}

	if (address & 0x0020) {
		prg <<= 1;
		if (address & 0x0040)
			prg++;
		setRom8KBank(4, prg*4+0);
		setRom8KBank(5, prg*4+1);
		setRom8KBank(6, prg*4+0);
		setRom8KBank(7, prg*4+1);
	} else {
		setRom8KBank(4, prg*4+0);
		setRom8KBank(5, prg*4+1);
		setRom8KBank(6, prg*4+2);
		setRom8KBank(7, prg*4+3);
	}

	ppuMapper->setVrom8KBank(((address&0x000F)<<2)|(data&0x03));

	if (address & 0x2000)
		ppuMapper->setMirroring(NesPpuMapper::Horizontal);
	else
		ppuMapper->setMirroring(NesPpuMapper::Vertical);
}

NES_MAPPER_PLUGIN_EXPORT(228, "Action 52")
