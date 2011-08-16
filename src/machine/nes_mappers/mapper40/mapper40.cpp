#include "mapper40.h"
#include <QDataStream>

CpuMapper40::CpuMapper40(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper40::reset() {
	ppuMapper = static_cast<PpuMapper40 *>(mapper()->ppuMapper());

	setRom8KBank(3, 6);
	setRom8KBanks(4, 5, 0, 7);
	if (ppuMapper->vromSize1KB())
		ppuMapper->setVrom8KBank(0);
	irq_enable = 0;
	irq_line = 0;
}

void CpuMapper40::writeHigh(quint16 address, quint8 data) {
	switch (address & 0xE000) {
	case 0x8000:
		irq_enable = 0;
		setIrqSignalOut(false);
		break;
	case 0xA000:
		irq_enable = 0xFF;
		irq_line = 37;
		setIrqSignalOut(false);
		break;
	case 0xC000:
		break;
	case 0xE000:
		setRom8KBank(6, data&0x07);
		break;
	}
}

PpuMapper40::PpuMapper40(NesMapper *mapper) :
	NesPpuMapper(mapper),
	cpuMapper(0) {
}

void PpuMapper40::reset() {
	cpuMapper = static_cast<CpuMapper40 *>(mapper()->cpuMapper());
}

void PpuMapper40::horizontalSync(int scanline) {
	Q_UNUSED(scanline)
	if (cpuMapper->irq_enable) {
		if (--cpuMapper->irq_line <= 0)
			cpuMapper->setIrqSignalOut(true);
	}
}

bool CpuMapper40::save(QDataStream &s) {
	if (!NesCpuMapper::save(s))
		return false;
	s << irq_enable;
	s << irq_line;
	return true;
}

bool CpuMapper40::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	s >> irq_enable;
	s >> irq_line;
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(40, "SMB2J")
