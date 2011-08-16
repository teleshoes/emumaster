#include "mapper230.h"

CpuMapper230::CpuMapper230(NesMapper *mapper) :
	NesCpuMapper(mapper),
	m_switch(false) {
	// TODO m_switch = !m_switch; on reset
	if (m_switch) {
		setRom8KBank(0, 0);
		setRom8KBank(1, 1);
		setRom8KBank(2, 14);
		setRom8KBank(3, 15);
	} else {
		setRom8KBank(0, 16);
		setRom8KBank(1, 17);
		setRom8KBank(2, romSize8KB() - 2);
		setRom8KBank(3, romSize8KB() - 1);
	}
}

void CpuMapper230::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(address)
	if (m_switch)
		setRom16KBank(0, data & 7);
	else {
		if(data & 0x20) {
			setRom16KBank(0, (data & 0x1F) + 8);
			setRom16KBank(1, (data & 0x1F) + 8);
		} else {
			setRomBank(((data >> 1) & 0x0F) + 4);
		}
		if (data & 0x40)
			mapper()->ppuMemory()->setMirroring(NesPpuMapper::Vertical);
		else
			mapper()->ppuMemory()->setMirroring(NesPpuMapper::Horizontal);
	}
}

NES_MAPPER_PLUGIN_EXPORT(230, "22-in-1")
