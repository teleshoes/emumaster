#include "mapper246.h"
#include "nesppu.h"
#include "nesdisk.h"
#include <QDataStream>

CpuMapper246::CpuMapper246(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper246::reset() {
	ppuMapper = mapper()->ppuMapper();

	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);
}

void CpuMapper246::writeHigh(quint16 address, quint8 data) {
	if (address >= 0x6000 && address < 0x8000) {
		switch( address) {
		case 0x6000:
			setRom8KBank(4, data);
			break;
		case 0x6001:
			setRom8KBank(5, data);
			break;
		case 0x6002:
			setRom8KBank(6, data);
			break;
		case 0x6003:
			setRom8KBank(7, data);
			break;
		case 0x6004:
			ppuMapper->setVrom2KBank(0, data);
			break;
		case 0x6005:
			ppuMapper->setVrom2KBank(2, data);
			break;
		case 0x6006:
			ppuMapper->setVrom2KBank(4, data);
			break;
		case 0x6007:
			ppuMapper->setVrom2KBank(6, data);
			break;
		default:
			writeDirect(address, data);
			break;
		}
	}
}

NES_MAPPER_PLUGIN_EXPORT(246, "Phone Serm Berm")
