#include "mapper201.h"
#include "nesppu.h"
#include "nesdisk.h"
#include <QDataStream>

CpuMapper201::CpuMapper201(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper201::reset() {
	ppuMapper = mapper()->ppuMapper();

	setRom16KBank(4, 0);
	setRom16KBank(6, 0);
	if (ppuMapper->vromSize1KB())
		ppuMapper->setVrom8KBank(0);
}

void CpuMapper201::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(data)

	quint8 bank = address & 0x03;
	if (!(address & 0x08))
		bank = 0;
	setRom32KBank(bank);
	ppuMapper->setVrom8KBank(bank);
}

NES_MAPPER_PLUGIN_EXPORT(201, "21-in-1")
