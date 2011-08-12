#include "mapper3.h"

CpuMapper3::CpuMapper3(NesMapper *mapper) :
	NesCpuMemoryMapper(mapper) {
}

void CpuMapper3::reset() {
	NesCpuMemoryMapper::reset();
//	TODO DWORD	crc = nes->rom->GetPROM_CRC();

//	if( crc == 0x2b72fe7e ) {	// Ganso Saiyuuki - Super Monkey Dai Bouken(J)
//		nes->SetRenderMethod( NES::TILE_RENDER );
//		nes->ppu->SetExtNameTableMode( TRUE );
//	}
}

void CpuMapper3::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(address)
	mapper()->ppuMemory()->setRomBank(data);
}

NES_MAPPER_PLUGIN_EXPORT(3, "CNROM")
