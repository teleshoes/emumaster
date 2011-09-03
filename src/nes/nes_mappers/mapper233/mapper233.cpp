#include "mapper233.h"
#include "nesppu.h"
#include "nesdisk.h"
#include <QDataStream>

CpuMapper233::CpuMapper233(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper233::reset() {
	ppuMapper = mapper()->ppuMapper();

	setRom32KBank(0);
}

void CpuMapper233::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(address)

	if (data & 0x20) {
		setRom8KBank(4, (data&0x1F)*2+0);
		setRom8KBank(5, (data&0x1F)*2+1);
		setRom8KBank(6, (data&0x1F)*2+0);
		setRom8KBank(7, (data&0x1F)*2+1);
	} else {
		quint8 bank = (data&0x1E)>>1;

		setRom8KBank(4, bank*4+0);
		setRom8KBank(5, bank*4+1);
		setRom8KBank(6, bank*4+2);
		setRom8KBank(7, bank*4+3);
	}

	if ((data&0xC0) == 0x00)
		ppuMapper->setMirroring(0, 0, 0, 1);
	else if ((data&0xC0) == 0x40)
		ppuMapper->setMirroring(NesPpuMapper::Vertical);
	else if ((data&0xC0) == 0x80)
		ppuMapper->setMirroring(NesPpuMapper::Horizontal);
	else
		ppuMapper->setMirroring(NesPpuMapper::SingleHigh);
}

NES_MAPPER_PLUGIN_EXPORT(233, "42-in-1")
