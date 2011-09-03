#include "mapper42.h"
#include <QDataStream>

CpuMapper42::CpuMapper42(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper42::reset() {
	ppuMapper = static_cast<PpuMapper42 *>(mapper()->ppuMapper());

	setRom8KBank(3, 0);
	setRom8KBanks(romSize8KB()-4, romSize8KB()-3, romSize8KB()-2, romSize8KB()-1);
	if (ppuMapper->vromSize1KB())
		ppuMapper->setVrom8KBank(0);
	irq_enable = 0;
	irq_counter = 0;
}

void CpuMapper42::writeHigh(quint16 address, quint8 data) {
	switch (address & 0xE003) {
	case 0xE000:
		setRom8KBank(3, data&0x0F);
		break;

	case 0xE001:
		ppuMapper->setMirroring(static_cast<NesPpuMapper::Mirroring>((data & 0x08) >> 3));
		break;

	case 0xE002:
		if (data&0x02) {
			irq_enable = 0xFF;
		} else {
			irq_enable = 0;
			irq_counter = 0;
		}
		setIrqSignalOut(false);
		break;
	}
}

PpuMapper42::PpuMapper42(NesMapper *mapper) :
	NesPpuMapper(mapper),
	cpuMapper(0) {
}

void PpuMapper42::reset() {
	cpuMapper = static_cast<CpuMapper42 *>(mapper()->cpuMapper());
}

void PpuMapper42::horizontalSync(int scanline) {
	Q_UNUSED(scanline)
	cpuMapper->setIrqSignalOut(false);
	if (cpuMapper->irq_enable) {
		if (cpuMapper->irq_counter < 215) {
			cpuMapper->irq_counter++;
		}
		if (cpuMapper->irq_counter == 215) {
			cpuMapper->irq_enable = 0;
			cpuMapper->setIrqSignalOut(true);
		}
	}
}

bool CpuMapper42::save(QDataStream &s) {
	if (!NesCpuMapper::save(s))
		return false;
	s << irq_enable;
	s << irq_counter;
	return true;
}

bool CpuMapper42::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	s >> irq_enable;
	s >> irq_counter;
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(42, "Mario Baby")
