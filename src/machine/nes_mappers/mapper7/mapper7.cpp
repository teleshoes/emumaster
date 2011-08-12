#include "mapper7.h"

CpuMapper7::CpuMapper7(NesMapper *mapper) :
	NesCpuMemoryMapper(mapper) {
}

void CpuMapper7::reset() {
	NesCpuMemoryMapper::reset();
	m_patch = 0;
	setRomBank(0);
	mapper()->ppuMemory()->setMirroring(NesPpuMemoryMapper::SingleLow);
	/* TODO DWORD	crc = nes->rom->GetPROM_CRC();
	if( crc == 0x3c9fe649 ) {	// WWF Wrestlemania Challenge(U)
		SetVRAM_Mirror( VRAM_VMIRROR );
		m_patch = 1;
	}
	if( crc == 0x09874777 ) {	// Marble Madness(U)
		nes->SetRenderMethod( NES::TILE_RENDER );
	}

	if( crc == 0x279710DC		// Battletoads (U)
	 || crc == 0xCEB65B06 ) {	// Battletoads Double Dragon (U)
		nes->SetRenderMethod( NES::PRE_ALL_RENDER );
		::memset( WRAM, 0, sizeof(WRAM) );
 } */
}

void CpuMapper7::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(address)
	setRomBank(data & 0x07);
	if (!m_patch) {
		if (data & 0x10)
			mapper()->ppuMemory()->setMirroring(NesPpuMemoryMapper::SingleHigh);
		else
			mapper()->ppuMemory()->setMirroring(NesPpuMemoryMapper::SingleLow);
	}
}

NES_MAPPER_PLUGIN_EXPORT(7, "AOROM/AMROM")
