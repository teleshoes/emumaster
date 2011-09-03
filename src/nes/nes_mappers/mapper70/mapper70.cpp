#include "mapper70.h"
#include "nesdisk.h"
#include "nesppu.h"

CpuMapper70::CpuMapper70(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper70::reset() {
	ppuMapper = mapper()->ppuMapper();

	patch = 0;

	quint32 crc = disk()->crc();
	if (crc == 0xa59ca2ef) {	// Kamen Rider Club(J)
		patch = 1;
		ppu()->setRenderMethod(NesPpu::PostAllRender);
	}
	if (crc == 0x10bb8f9a) {	// Family Trainer - Manhattan Police(J)
		patch = 1;
	}
	if (crc == 0x0cd00488) {	// Space Shadow(J)
		patch = 1;
	}
	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);
	ppuMapper->setVrom8KBank(0);
}

void CpuMapper70::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(address)
	setRom16KBank(4, (data >> 4) & 7);
	ppuMapper->setVrom8KBank(data & 0x0F);
	if (patch) {
		if (data & 0x80)
			ppuMapper->setMirroring(NesPpuMapper::Horizontal);
		else
			ppuMapper->setMirroring(NesPpuMapper::Vertical);
	} else {
		if (data & 0x80)
			ppuMapper->setMirroring(NesPpuMapper::SingleHigh);
		else
			ppuMapper->setMirroring(NesPpuMapper::SingleLow);
	}
}

NES_MAPPER_PLUGIN_EXPORT(70, "Bandai 74161")
