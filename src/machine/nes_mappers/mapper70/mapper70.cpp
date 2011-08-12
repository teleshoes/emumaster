#include "mapper70.h"
#include "nesppumemorymapper.h"

CpuMapper70::CpuMapper70(NesMapper *mapper) :
	NesCpuMemoryMapper(mapper) {
	m_patch = false;
//	TODO DWORD	crc = nes->rom->GetPROM_CRC();
//	if( crc == 0xa59ca2ef ) {	// Kamen Rider Club(J)
//		patch = 1;
//		nes->SetRenderMethod( NES::POST_ALL_RENDER );
//	}
//	if( crc == 0x10bb8f9a ) {	// Family Trainer - Manhattan Police(J)
//		patch = 1;
//	}
//	if( crc == 0x0cd00488 ) {	// Space Shadow(J)
//		patch = 1;
//	}
	setRom16KBank(0, 0);
	setRom16KBank(1, romSize16KB() - 1);
}

void CpuMapper70::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(address)
	setRom16KBank(0, (data >> 4) & 7);
	mapper()->ppuMemory()->setRomBank(data & 0x0F);
	if (m_patch) {
		if (data & 0x80)
			mapper()->ppuMemory()->setMirroring(NesPpuMemoryMapper::Horizontal);
		else
			mapper()->ppuMemory()->setMirroring(NesPpuMemoryMapper::Vertical);
	} else {
		if (data & 0x80)
			mapper()->ppuMemory()->setMirroring(NesPpuMemoryMapper::SingleHigh);
		else
			mapper()->ppuMemory()->setMirroring(NesPpuMemoryMapper::SingleLow);
	}
}

NES_MAPPER_PLUGIN_EXPORT(70, "Bandai 74161")
