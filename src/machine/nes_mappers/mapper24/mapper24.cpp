#include "mapper24.h"
#include "nesppu.h"
#include "nesdisk.h"
#include <QDataStream>

CpuMapper24::CpuMapper24(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper24::reset() {
	ppuMapper = mapper()->ppuMapper();

	irq_enable = 0;
	irq_counter = 0;
	irq_latch = 0;
	irq_clock = 0;

	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);
	if (ppuMapper->vromSize1KB())
		ppuMapper->setVrom8KBank(0);

	ppu()->setRenderMethod(NesPpu::PostRender);
	// TODO nes->apu->SelectExSound( 1);
}

void CpuMapper24::writeHigh(quint16 address, quint8 data) {
	switch (address & 0xF003) {
	case 0x8000:
		setRom16KBank(4, data);
		break;

	case 0x9000: case 0x9001: case 0x9002:
	case 0xA000: case 0xA001: case 0xA002:
	case 0xB000: case 0xB001: case 0xB002:
		// TODO nes->apu->ExWrite( addr, data);
		break;

	case 0xB003:
		ppuMapper->setMirroring(static_cast<NesPpuMapper::Mirroring>((data >> 3) & 0x03));
		break;

	case 0xC000:
		setRom8KBank(6, data);
		break;

	case 0xD000:
		ppuMapper->setVrom1KBank(0, data);
		break;

	case 0xD001:
		ppuMapper->setVrom1KBank(1, data);
		break;

	case 0xD002:
		ppuMapper->setVrom1KBank(2, data);
		break;

	case 0xD003:
		ppuMapper->setVrom1KBank(3, data);
		break;

	case 0xE000:
		ppuMapper->setVrom1KBank(4, data);
		break;

	case 0xE001:
		ppuMapper->setVrom1KBank(5, data);
		break;

	case 0xE002:
		ppuMapper->setVrom1KBank(6, data);
		break;

	case 0xE003:
		ppuMapper->setVrom1KBank(7, data);
		break;

	case 0xF000:
		irq_latch = data;
		break;
	case 0xF001:
		irq_enable = data & 0x03;
		if (irq_enable & 0x02) {
			irq_counter = irq_latch;
			irq_clock = 0;
		}
		setIrqSignalOut(false);
		break;
	case 0xF002:
		irq_enable = (irq_enable & 0x01) * 3;
		setIrqSignalOut(false);
		break;
	}
}

void CpuMapper24::clock(uint cycles) {
	if (irq_enable & 0x02) {
		if ((irq_clock+=cycles) >= 0x72) {
			irq_clock -= 0x72;
			if (irq_counter == 0xFF) {
				irq_counter = irq_latch;
				setIrqSignalOut(true);
			} else {
				irq_counter++;
			}
		}
	}
}

bool CpuMapper24::save(QDataStream &s) {
	if (!NesCpuMapper::save(s))
		return false;
	s << irq_enable;
	s << irq_counter;
	s << irq_latch;
	s << irq_clock;
	return true;
}

bool CpuMapper24::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	s >> irq_enable;
	s >> irq_counter;
	s >> irq_latch;
	s >> irq_clock;
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(24, "Konami VRC6(Normal)")
