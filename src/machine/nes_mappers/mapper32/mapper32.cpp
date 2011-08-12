#include "mapper32.h"
#include <QDataStream>

CpuMapper32::CpuMapper32(NesMapper *mapper) :
	NesCpuMemoryMapper(mapper) {
}

void CpuMapper32::reset() {
	NesCpuMemoryMapper::reset();
	m_patch = 0;
	m_reg = 0;
	setRom16KBank(0, 0);
	setRom16KBank(1, romSize16KB()-1);
	if (mapper()->ppuMemory()->romSize())
		mapper()->ppuMemory()->setRomBank(0);
	/* TODO DWORD	crc = nes->rom->GetPROM_CRC();
	// For Major League(J)
	if (crc == 0xc0fed437) {
		patch = 1;
	}
	// For Ai Sensei no Oshiete - Watashi no Hoshi(J)
	if (crc == 0xfd3fc292) {
		SetPROM_32K_Bank( 30, 31, 30, 31);
	}*/
}

void CpuMapper32::writeHigh(quint16 address, quint8 data) {
	switch (address & 0xF000) {
	case 0x8000:
		if (m_reg & 0x02)
			setRom8KBank(2, data);
		else
			setRom8KBank(0, data);
		break;
	case 0x9000:
		m_reg = data;
		if (data & 0x01)
			mapper()->ppuMemory()->setMirroring(NesPpuMemoryMapper::Horizontal);
		else
			mapper()->ppuMemory()->setMirroring(NesPpuMemoryMapper::Vertical);
		break;
	case 0xA000:
		setRom8KBank(1, data);
		break;
	}
	switch (address & 0xF007) {
	case 0xB000:
	case 0xB001:
	case 0xB002:
	case 0xB003:
	case 0xB004:
	case 0xB005:
		mapper()->ppuMemory()->setRom1KBank(address & 0x0007, data);
		break;
	case 0xB006:
		mapper()->ppuMemory()->setRom1KBank(6, data);
		if (m_patch && (data & 0x40))
			mapper()->ppuMemory()->setMirroring(0, 0, 0, 1);
		break;
	case 0xB007:
		mapper()->ppuMemory()->setRom1KBank(7, data);
		if (m_patch && (data & 0x40))
			mapper()->ppuMemory()->setMirroring(NesPpuMemoryMapper::SingleLow);
		break;
	}
}

void CpuMapper32::save(QDataStream &s) {
	NesCpuMemoryMapper::save(s);
	s << m_reg;
}

bool CpuMapper32::load(QDataStream &s) {
	if (!NesCpuMemoryMapper::load(s))
		return false;
	s >> m_reg;
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(32, "Irem G101")
