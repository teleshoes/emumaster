#include "mapper68.h"
#include "nesdisk.h"
#include "nesppu.h"
#include <QDataStream>

// TODO coin ???

CpuMapper68::CpuMapper68(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper68::reset() {
	ppuMapper = mapper()->ppuMapper();

	reg[0] = reg[1] = reg[2] = reg[3] = 0;

	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);
}

void CpuMapper68::writeHigh(quint16 address, quint8 data) {
	switch (address & 0xF000) {
	case 0x8000:
		ppuMapper->setVrom2KBank(0, data);
		break;
	case 0x9000:
		ppuMapper->setVrom2KBank(2, data);
		break;
	case 0xA000:
		ppuMapper->setVrom2KBank(4, data);
		break;
	case 0xB000:
		ppuMapper->setVrom2KBank(6, data);
		break;

	case 0xC000:
		reg[2] = data;
		updateBanks();
		break;
	case 0xD000:
		reg[3] = data;
		updateBanks();
		break;
	case 0xE000:
		reg[0] = (data & 0x10)>>4;
		reg[1] = data & 0x03;
		updateBanks();
		break;

	case 0xF000:
		setRom16KBank(4, data);
		break;
	}
}

void CpuMapper68::updateBanks() {
	if (reg[0]) {
		switch (reg[1]) {
		case 0:
			ppuMapper->setVrom1KBank( 8, reg[2]+0x80);
			ppuMapper->setVrom1KBank( 9, reg[3]+0x80);
			ppuMapper->setVrom1KBank(10, reg[2]+0x80);
			ppuMapper->setVrom1KBank(11, reg[3]+0x80);
			break;
		case 1:
			ppuMapper->setVrom1KBank( 8, reg[2]+0x80);
			ppuMapper->setVrom1KBank( 9, reg[2]+0x80);
			ppuMapper->setVrom1KBank(10, reg[3]+0x80);
			ppuMapper->setVrom1KBank(11, reg[3]+0x80);
			break;
		case 2:
			ppuMapper->setVrom1KBank( 8, reg[2]+0x80);
			ppuMapper->setVrom1KBank( 9, reg[2]+0x80);
			ppuMapper->setVrom1KBank(10, reg[2]+0x80);
			ppuMapper->setVrom1KBank(11, reg[2]+0x80);
			break;
		case 3:
			ppuMapper->setVrom1KBank( 8, reg[3]+0x80);
			ppuMapper->setVrom1KBank( 9, reg[3]+0x80);
			ppuMapper->setVrom1KBank(10, reg[3]+0x80);
			ppuMapper->setVrom1KBank(11, reg[3]+0x80);
			break;
		}
	} else {
		ppuMapper->setMirroring(static_cast<NesPpuMapper::Mirroring>(reg[1] & 0x03));
	}
}

bool CpuMapper68::save(QDataStream &s) {
	if (!NesCpuMapper::save(s))
		return false;
	s << reg[0] << reg[1] << reg[2];
	return true;
}

bool CpuMapper68::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	s >> reg[0] >> reg[1] >> reg[2];
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(68, "SunSoft Mapper 4")
