#include "mapper50.h"
#include <QDataStream>

CpuMapper50::CpuMapper50(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper50::reset() {
	ppuMapper = static_cast<PpuMapper50 *>(mapper()->ppuMapper());

	irq_enable = 0;
	setRom8KBank(3, 15);
	setRom8KBank(4, 8);
	setRom8KBank(5, 9);
	setRom8KBank(6, 0);
	setRom8KBank(7, 11);
	if (ppuMapper->vromSize1KB())
		ppuMapper->setVrom8KBank(0);
}

void CpuMapper50::writeEx(quint16 address, quint8 data) {
	if ((address & 0xE060) == 0x4020) {
		if (address & 0x0100) {
			irq_enable = data & 0x01;
			setIrqSignalOut(false);
		} else {
			setRom8KBank(6, (data&0x08)|((data&0x01)<<2)|((data&0x06)>>1));
		}
	}
}

void CpuMapper50::writeLow(quint16 address, quint8 data) {
	if ((address & 0xE060) == 0x4020) {
		if (address & 0x0100) {
			irq_enable = data & 0x01;
			setIrqSignalOut(false);
		} else {
			setRom8KBank(6, (data&0x08)|((data&0x01)<<2)|((data&0x06)>>1));
		}
	}
}

PpuMapper50::PpuMapper50(NesMapper *mapper) :
	NesPpuMapper(mapper),
	cpuMapper(0) {
}

void PpuMapper50::reset() {
	cpuMapper = static_cast<CpuMapper50 *>(mapper()->cpuMapper());
}

void PpuMapper50::horizontalSync(int scanline) {
	if (cpuMapper->irq_enable) {
		if (scanline == 21)
			cpuMapper->setIrqSignalOut(true);
	}
}

bool CpuMapper50::save(QDataStream &s) {
	if (!NesCpuMapper::save(s))
		return false;
	s << irq_enable;
	return true;
}

bool CpuMapper50::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	s >> irq_enable;
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(50, "SMB2J")
