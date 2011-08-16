#include "mapper60.h"
#include "nesdisk.h"
#include <QDataStream>

CpuMapper60::CpuMapper60(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper60::reset() {
	ppuMapper = mapper()->ppuMapper();

	patch = 0;
	quint32 crc = disk()->crc();
	if (crc == 0xf9c484a0) {	// Reset Based 4-in-1(Unl)
		setRom16KBank(4, game_sel);
		setRom16KBank(6, game_sel);
		ppuMapper->setVrom8KBank(game_sel);
		game_sel++;
		game_sel &= 3;
	} else {
		patch = 1;
		setRom32KBank(0);
		ppuMapper->setVrom8KBank(0);
	}
}

void CpuMapper60::writeHigh(quint16 address, quint8 data) {
	if (patch) {
		if (address & 0x80) {
			setRom16KBank(4, (address & 0x70) >> 4);
			setRom16KBank(6, (address & 0x70) >> 4);
		} else {
			setRom32KBank((address & 0x70) >> 5);
		}
		ppuMapper->setVrom8KBank(address & 0x07);
		ppuMapper->setMirroring(static_cast<NesPpuMapper::Mirroring>((data & 0x08) >> 3));
	}
}

bool CpuMapper60::save(QDataStream &s) {
	if (!NesCpuMapper::save(s))
		return false;
	s << game_sel;
	return true;
}

bool CpuMapper60::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	s >> game_sel;
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(60, "?")
