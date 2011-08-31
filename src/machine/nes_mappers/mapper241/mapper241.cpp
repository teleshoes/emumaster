#include "mapper241.h"
#include "nesppu.h"
#include "nesdisk.h"
#include <QDataStream>

CpuMapper241::CpuMapper241(NesMapper *mapper) :
	NesCpuMapper(mapper) {
}

void CpuMapper241::reset() {
	setRom32KBank(0);
	NesPpuMapper *ppuMapper = mapper()->ppuMapper();
	if (ppuMapper->vromSize1KB())
		ppuMapper->setVrom8KBank(0);
}

void CpuMapper241::writeHigh(quint16 address, quint8 data) {
	if (address == 0x8000)
		setRom32KBank(data);
}

NES_MAPPER_PLUGIN_EXPORT(241, "Fon Serm Bon")
