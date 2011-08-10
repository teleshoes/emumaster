#include "mapper246.h"

CpuMapper246::CpuMapper246(NesMapper *mapper) :
	NesCpuMemoryMapper(mapper) {
	setRom16KBank(0, 0);
	setRom16KBank(1, romSize16KB() - 1);
}

void CpuMapper246::write(quint16 address, quint8 data) {
	if (address >= 0x6000 && address < 0x8000) {
		switch(address) {
		case 0x6000: setRom8KBank(0, data); break;
		case 0x6001: setRom8KBank(1, data); break;
		case 0x6002: setRom8KBank(2, data); break;
		case 0x6003: setRom8KBank(3, data); break;
		case 0x6004: mapper()->ppuMemory()->setRom2KBank(0, data); break;
		case 0x6005: mapper()->ppuMemory()->setRom2KBank(1, data); break;
		case 0x6006: mapper()->ppuMemory()->setRom2KBank(2, data); break;
		case 0x6007: mapper()->ppuMemory()->setRom2KBank(3, data); break;
		default:
			NesCpuMemoryMapper::write(address, data);
			break;
		}
	} else {
		NesCpuMemoryMapper::write(address, data);
	}
}

NES_MAPPER_PLUGIN_SOURCE(246, "Phone Serm Berm")
