#include "mapper231.h"
#include "nesppu.h"
#include "nesdisk.h"
#include <QDataStream>

CpuMapper231::CpuMapper231(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper231::reset() {
	ppuMapper = mapper()->ppuMapper();

	setRom32KBank(0);
	if (ppuMapper->vromSize1KB())
		ppuMapper->setVrom8KBank(0);
}

void CpuMapper231::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(data)

	if (address & 0x0020) {
		setRom32KBank(quint8(address>>1));
	} else {
		quint8 bank = address & 0x1E;
		setRom8KBank(4, bank*2+0);
		setRom8KBank(5, bank*2+1);
		setRom8KBank(6, bank*2+0);
		setRom8KBank(7, bank*2+1);
	}

	if (address & 0x0080)
		ppuMapper->setMirroring(NesPpuMapper::Horizontal);
	else
		ppuMapper->setMirroring(NesPpuMapper::Vertical);
}

NES_MAPPER_PLUGIN_EXPORT(231, "20-in-1")
