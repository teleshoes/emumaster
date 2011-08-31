#include "mapper252.h"
#include "nesppu.h"
#include "nesdisk.h"
#include <QDataStream>

CpuMapper252::CpuMapper252(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper252::reset() {
	ppuMapper = mapper()->ppuMapper();

	for (int i = 0; i < 8; i++)
		reg[i] = i;
	reg[8] = 0;

	irq_enable = 0;
	irq_counter = 0;
	irq_latch = 0;
	irq_clock = 0;
	irq_occur = 0;

	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);
	ppuMapper->setVrom8KBank(0);

	ppu()->setRenderMethod(NesPpu::PostRender);
}

void CpuMapper252::writeHigh(quint16 address, quint8 data) {
	if ((address & 0xF000) == 0x8000) {
		setRom8KBank(4, data);
		return;
	}
	if ((address & 0xF000) == 0xA000) {
		setRom8KBank(5, data);
		return;
	}
	switch (address & 0xF00C) {
	case 0xB000:
		reg[0] = (reg[0] & 0xF0) | (data & 0x0F);
		ppuMapper->setVrom1KBank(0, reg[0]);
		break;
	case 0xB004:
		reg[0] = (reg[0] & 0x0F) | ((data & 0x0F) << 4);
		ppuMapper->setVrom1KBank(0, reg[0]);
		break;
	case 0xB008:
		reg[1] = (reg[1] & 0xF0) | (data & 0x0F);
		ppuMapper->setVrom1KBank(1, reg[1]);
		break;
	case 0xB00C:
		reg[1] = (reg[1] & 0x0F) | ((data & 0x0F) << 4);
		ppuMapper->setVrom1KBank(1, reg[1]);
		break;

	case 0xC000:
		reg[2] = (reg[2] & 0xF0) | (data & 0x0F);
		ppuMapper->setVrom1KBank(2, reg[2]);
		break;
	case 0xC004:
		reg[2] = (reg[2] & 0x0F) | ((data & 0x0F) << 4);
		ppuMapper->setVrom1KBank(2, reg[2]);
		break;
	case 0xC008:
		reg[3] = (reg[3] & 0xF0) | (data & 0x0F);
		ppuMapper->setVrom1KBank(3, reg[3]);
		break;
	case 0xC00C:
		reg[3] = (reg[3] & 0x0F) | ((data & 0x0F) << 4);
		ppuMapper->setVrom1KBank(3, reg[3]);
		break;

	case 0xD000:
		reg[4] = (reg[4] & 0xF0) | (data & 0x0F);
		ppuMapper->setVrom1KBank(4, reg[4]);
		break;
	case 0xD004:
		reg[4] = (reg[4] & 0x0F) | ((data & 0x0F) << 4);
		ppuMapper->setVrom1KBank(4, reg[4]);
		break;
	case 0xD008:
		reg[5] = (reg[5] & 0xF0) | (data & 0x0F);
		ppuMapper->setVrom1KBank(5, reg[5]);
		break;
	case 0xD00C:
		reg[5] = (reg[5] & 0x0F) | ((data & 0x0F) << 4);
		ppuMapper->setVrom1KBank(5, reg[5]);
		break;

	case 0xE000:
		reg[6] = (reg[6] & 0xF0) | (data & 0x0F);
		ppuMapper->setVrom1KBank(6, reg[6]);
		break;
	case 0xE004:
		reg[6] = (reg[6] & 0x0F) | ((data & 0x0F) << 4);
		ppuMapper->setVrom1KBank(6, reg[6]);
		break;
	case 0xE008:
		reg[7] = (reg[7] & 0xF0) | (data & 0x0F);
		ppuMapper->setVrom1KBank(7, reg[7]);
		break;
	case 0xE00C:
		reg[7] = (reg[7] & 0x0F) | ((data & 0x0F) << 4);
		ppuMapper->setVrom1KBank(7, reg[7]);
		break;

	case 0xF000:
		irq_latch = (irq_latch & 0xF0) | (data & 0x0F);
		irq_occur = 0;
		break;
	case 0xF004:
		irq_latch = (irq_latch & 0x0F) | ((data & 0x0F) << 4);
		irq_occur = 0;
		break;

	case 0xF008:
		irq_enable = data & 0x03;
		if (irq_enable & 0x02) {
			irq_counter = irq_latch;
			irq_clock = 0;
		}
		irq_occur = 0;
		setIrqSignalOut(false);
		break;

	case 0xF00C:
		irq_enable = (irq_enable & 0x01) * 3;
		irq_occur = 0;
		setIrqSignalOut(false);
		break;
	}
}

void CpuMapper252::clock(uint cycles) {
	if (irq_enable & 0x02) {
		if ((irq_clock+=cycles) >= 0x72) {
			irq_clock -= 0x72;
			if (irq_counter == 0xFF) {
				irq_occur = 0xFF;
				irq_counter = irq_latch;
				irq_enable = (irq_enable & 0x01) * 3;

				setIrqSignalOut(true);
			} else {
				irq_counter++;
			}
		}
	}
}

bool CpuMapper252::save(QDataStream &s) {
	if (!NesCpuMapper::save(s))
		return false;
	for (int i = 0; i < 9; i++)
		s << reg[i];
	s << irq_enable;
	s << irq_counter;
	s << irq_latch;
	s << irq_occur;
	s << irq_clock;
	return true;
}

bool CpuMapper252::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	for (int i = 0; i < 9; i++)
		s >> reg[i];
	s >> irq_enable;
	s >> irq_counter;
	s >> irq_latch;
	s >> irq_occur;
	s >> irq_clock;
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(252, "-")
