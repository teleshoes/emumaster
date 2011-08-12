#include "mapper22.h"

CpuMapper22::CpuMapper22(NesMapper *mapper) :
	NesCpuMemoryMapper(mapper),
	m_ppuMapper(0) {
}

void CpuMapper22::reset() {
	NesCpuMemoryMapper::reset();
	m_ppuMapper = mapper()->ppuMemory();
	setRom16KBank(0, 0);
	setRom16KBank(0, romSize16KB()-1);
}

void CpuMapper22::writeHigh(quint16 address, quint8 data) {
	switch (address) {
	case 0x8000:
		setRom8KBank(0, data);
		break;
	case 0x9000:
		data &= 0x03;
		if (data == 0)
			m_ppuMapper->setMirroring(NesPpuMemoryMapper::Vertical);
		else if (data == 1)
			m_ppuMapper->setMirroring(NesPpuMemoryMapper::Horizontal);
		else if (data == 2)
			m_ppuMapper->setMirroring(NesPpuMemoryMapper::SingleHigh);
		else
			m_ppuMapper->setMirroring(NesPpuMemoryMapper::SingleLow);
		break;
	case 0xA000: setRom8KBank(1, data); break;
	case 0xB000: m_ppuMapper->setRom1KBank(0, data >> 1); break;
	case 0xB001: m_ppuMapper->setRom1KBank(1, data >> 1); break;
	case 0xC000: m_ppuMapper->setRom1KBank(2, data >> 1); break;
	case 0xC001: m_ppuMapper->setRom1KBank(3, data >> 1); break;
	case 0xD000: m_ppuMapper->setRom1KBank(4, data >> 1); break;
	case 0xD001: m_ppuMapper->setRom1KBank(5, data >> 1); break;
	case 0xE000: m_ppuMapper->setRom1KBank(6, data >> 1); break;
	case 0xE001: m_ppuMapper->setRom1KBank(7, data >> 1); break;
	}
}

NES_MAPPER_PLUGIN_EXPORT(22, "Konami VRC2 type A")
