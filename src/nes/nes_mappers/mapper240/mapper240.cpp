#include "mapper240.h"
#include "nesppu.h"
#include "nesdisk.h"
#include <QDataStream>

CpuMapper240::CpuMapper240(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper240::reset() {
	ppuMapper = mapper()->ppuMapper();

	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);
	if (ppuMapper->vromSize1KB())
		ppuMapper->setVrom8KBank(0);
}

void CpuMapper240::writeLow(quint16 address, quint8 data) {
	if (address>=0x4020 && address < 0x6000) {
		setRom32KBank((data & 0xF0) >> 4);
		ppuMapper->setVrom8KBank(data & 0xF);
	}
}

NES_MAPPER_PLUGIN_EXPORT(240, "Gen Ke Le Zhuan")
