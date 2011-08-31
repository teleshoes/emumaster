#include "mapper229.h"
#include "nesppu.h"
#include "nesdisk.h"
#include <QDataStream>

CpuMapper229::CpuMapper229(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper229::reset() {
	ppuMapper = mapper()->ppuMapper();

	setRom32KBank(0);
	ppuMapper->setVrom8KBank(0);
}

void CpuMapper229::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(data)

	if (address & 0x001E) {
		quint8 prg = address & 0x001F;

		setRom8KBank(4, prg*2+0);
		setRom8KBank(5, prg*2+1);
		setRom8KBank(6, prg*2+0);
		setRom8KBank(7, prg*2+1);

		ppuMapper->setVrom8KBank(address & 0x0FFF);
	} else {
		setRom32KBank(0);
		ppuMapper->setVrom8KBank(0);
	}
	if (address & 0x0020)
		ppuMapper->setMirroring(NesPpuMapper::Horizontal);
	else
		ppuMapper->setMirroring(NesPpuMapper::Vertical);
}

NES_MAPPER_PLUGIN_EXPORT(229, "31-in-1")
