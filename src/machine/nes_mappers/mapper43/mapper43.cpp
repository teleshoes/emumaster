#include "mapper43.h"
#include <QDataStream>

CpuMapper43::CpuMapper43(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper43::reset() {
	ppuMapper = static_cast<PpuMapper43 *>(mapper()->ppuMapper());

	setRom8KBank(3, 2);
	setRom8KBanks(1, 0, 4, 9);
	if (ppuMapper->vromSize1KB())
		ppuMapper->setVrom8KBank(0);
	irq_enable = 0xFF;
	irq_counter = 0;
}

quint8 CpuMapper43::readLow(quint16 address) {
	if (0x5000 <= address && address < 0x6000)
		return	m_rom[0x2000*8+0x1000+(address-0x5000)];
	return address >> 8;
}

void CpuMapper43::writeEx(quint16 address, quint8 data) {
	if ((address&0xF0FF) == 0x4022) {
		switch (data&0x07) {
		case 0x00:
		case 0x02:
		case 0x03:
		case 0x04:
			setRom8KBank(6, 4);
			break;
		case 0x01:
			setRom8KBank(6, 3);
			break;
		case 0x05:
			setRom8KBank(6, 7);
			break;
		case 0x06:
			setRom8KBank(6, 5);
			break;
		case 0x07:
			setRom8KBank(6, 6);
			break;
		}
	}
}

void CpuMapper43::writeLow(quint16 address, quint8 data) {
	if ((address&0xF0FF) == 0x4022)
		writeEx(address, data);
}

void CpuMapper43::writeHigh(quint16 address, quint8 data) {
	if (address == 0x8122) {
		if (data & 0x03) {
			irq_enable = 1;
		} else {
			irq_counter = 0;
			irq_enable = 0;
		}
		setIrqSignalOut(false);
	}
}

PpuMapper43::PpuMapper43(NesMapper *mapper) :
	NesPpuMapper(mapper),
	cpuMapper(0) {
}

void PpuMapper43::reset() {
	cpuMapper = static_cast<CpuMapper43 *>(mapper()->cpuMapper());
}

void PpuMapper43::horizontalSync(int scanline) {
	Q_UNUSED(scanline)
	cpuMapper->setIrqSignalOut(false);
	if (cpuMapper->irq_enable) {
		cpuMapper->irq_counter += 341;
		if (cpuMapper->irq_counter >= 12288) {
			cpuMapper->irq_counter = 0;
			cpuMapper->setIrqSignalOut(true);
		}
	}
}

bool CpuMapper43::save(QDataStream &s) {
	if (!NesCpuMapper::save(s))
		return false;
	s << irq_enable;
	s << irq_counter;
	return true;
}

bool CpuMapper43::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	s >> irq_enable;
	s >> irq_counter;
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(43, "SMB2J")
