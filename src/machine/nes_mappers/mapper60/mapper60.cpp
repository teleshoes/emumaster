#include "mapper60.h"
#include <QDataStream>

CpuMapper60::CpuMapper60(NesMapper *mapper) :
	NesCpuMapper(mapper) {
}

void CpuMapper60::reset() {
	NesCpuMapper::reset();
	m_patch = 0;
	/* TODO DWORD	crc = nes->rom->GetPROM_CRC();
	if (crc == 0xf9c484a0) {	// Reset Based 4-in-1(Unl)
		SetPROM_16K_Bank( 4, game_sel);
		SetPROM_16K_Bank( 6, game_sel);
		SetVROM_8K_Bank( game_sel);
		game_sel++;
		game_sel &= 3;
	} else {
		patch = 1;
		SetPROM_32K_Bank( 0);
		SetVROM_8K_Bank( 0);
	}*/
}

void CpuMapper60::writeHigh(quint16 address, quint8 data) {
	if (m_patch) {
		if (address & 0x80) {
			setRom16KBank(0, (address & 0x70) >> 4);
			setRom16KBank(1, (address & 0x70) >> 4);
		} else {
			setRomBank((address & 0x70) >> 5);
		}
		NesPpuMapper *ppuMapper = mapper()->ppuMemory();
		ppuMapper->setRomBank(address & 0x07);
		if (data & 0x08)
			ppuMapper->setMirroring(NesPpuMapper::Vertical);
		else
			ppuMapper->setMirroring(NesPpuMapper::Horizontal);
	}
}

void CpuMapper60::save(QDataStream &s) {
	NesCpuMapper::save(s);
	s << game_sel;
}

bool CpuMapper60::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	s >> game_sel;
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(60, "60")
