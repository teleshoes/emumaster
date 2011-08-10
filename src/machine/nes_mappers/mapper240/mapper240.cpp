#include "mapper240.h"

CpuMapper240::CpuMapper240(NesMapper *mapper) :
	NesCpuMemoryMapper(mapper) {
	setRom16KBank(0, 0);
	setRom16KBank(1, romSize16KB() - 1);
}

void CpuMapper240::write(quint16 address, quint8 data) {
	if(address >= 0x4020 && address < 0x6000) {
		setRomBank((data & 0xF0) >> 4);
		mapper()->cpuMemory()->setRomBank(data & 0xF);
	} else {
		NesCpuMemoryMapper::write(address, data);
	}
}

NES_MAPPER_PLUGIN_SOURCE(240, "Gen Ke Le Zhuan")
