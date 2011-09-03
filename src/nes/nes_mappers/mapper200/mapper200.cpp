#include "mapper200.h"
#include "nesppu.h"
#include "nesdisk.h"
#include <QDataStream>

CpuMapper200::CpuMapper200(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper200::reset() {
	ppuMapper = mapper()->ppuMapper();

	setRom16KBank(4, 0);
	setRom16KBank(6, 0);
	if (ppuMapper->vromSize1KB())
		ppuMapper->setVrom8KBank(0);
}

void CpuMapper200::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(data)

	setRom16KBank(4, address & 0x07);
	setRom16KBank(6, address & 0x07);
	ppuMapper->setVrom8KBank(address & 0x07);

	if (address & 0x01)
		ppuMapper->setMirroring(NesPpuMapper::Vertical);
	else
		ppuMapper->setMirroring(NesPpuMapper::Horizontal);
}

NES_MAPPER_PLUGIN_EXPORT(200, "1200-in-1")
