#include "mapper48.h"
#include "nesppu.h"
#include <QDataStream>

CpuMapper48::CpuMapper48(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper48::reset() {
	ppuMapper = static_cast<PpuMapper48 *>(mapper()->ppuMapper());

	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);
	if (ppuMapper->vromSize1KB())
		ppuMapper->setVrom8KBank(0);

	reg = 0;
	irq_enable = 0;
	irq_counter = 0;
}

void CpuMapper48::writeHigh(quint16 address, quint8 data) {
	switch (address) {
	case 0x8000:
		if (!reg)
			ppuMapper->setMirroring(static_cast<NesPpuMapper::Mirroring>((data & 0x40) >> 6));
		setRom8KBank(4, data);
		break;
	case 0x8001:
		setRom8KBank(5, data);
		break;

	case 0x8002:
		ppuMapper->setVrom2KBank(0, data);
		break;
	case 0x8003:
		ppuMapper->setVrom2KBank(2, data);
		break;
	case 0xA000:
		ppuMapper->setVrom1KBank(4, data);
		break;
	case 0xA001:
		ppuMapper->setVrom1KBank(5, data);
		break;
	case 0xA002:
		ppuMapper->setVrom1KBank(6, data);
		break;
	case 0xA003:
		ppuMapper->setVrom1KBank(7, data);
		break;

	case 0xC000:
		irq_counter = data;
		irq_enable = 0;
		break;

	case 0xC001:
		irq_counter = data;
		irq_enable = 1;
		break;

	case 0xC002:
		break;
	case 0xC003:
		break;

	case 0xE000:
		ppuMapper->setMirroring(static_cast<NesPpuMapper::Mirroring>((data & 0x40) >> 6));
		reg = 1;
		break;
	}
}

PpuMapper48::PpuMapper48(NesMapper *mapper) :
	NesPpuMapper(mapper),
	cpuMapper(0) {
}

void PpuMapper48::reset() {
	cpuMapper = static_cast<CpuMapper48 *>(mapper()->cpuMapper());
}

void PpuMapper48::horizontalSync(int scanline) {
	if (scanline < NesPpu::VisibleScreenHeight && cpuMapper->ppu()->registers()->isDisplayOn()) {
		if (cpuMapper->irq_enable) {
			if (cpuMapper->irq_counter == 0xFF)
				cpuMapper->setIrqSignalOut(true); // TODO need to be cleared somewhere (trigger in virtuanes)
			cpuMapper->irq_counter++;
		}
	}
}

bool CpuMapper48::save(QDataStream &s) {
	if (!NesCpuMapper::save(s))
		return false;
	s << reg;
	s << irq_enable;
	s << irq_counter;
	return true;
}

bool CpuMapper48::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	s >> reg;
	s >> irq_enable;
	s >> irq_counter;
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(48, "Taito TC190V")
