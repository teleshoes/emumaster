#include "mapper21.h"
#include "nesppu.h"
#include "nesdisk.h"
#include <QDataStream>

CpuMapper21::CpuMapper21(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper21::reset() {
	ppuMapper = mapper()->ppuMapper();

	for (int i = 0; i < 8; i++)
		reg[i] = i;
	reg[8] = 0;

	irq_enable = 0;
	irq_counter = 0;
	irq_latch = 0;
	irq_clock = 0;

	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);
}

void CpuMapper21::writeHigh(quint16 address, quint8 data) {
	switch (address & 0xF0CF) {
	case 0x8000:
		if(reg[8] & 0x02) {
			setRom8KBank(6, data);
		} else {
			setRom8KBank(4, data);
		}
		break;
	case 0xA000:
		setRom8KBank(5, data);
		break;

	case 0x9000:
		ppuMapper->setMirroring(static_cast<NesPpuMapper::Mirroring>(data & 3));
		break;

	case 0x9002:
	case 0x9080:
		reg[8] = data;
		break;

	case 0xB000:
		reg[0] = (reg[0] & 0xF0) | (data & 0x0F);
		ppuMapper->setVrom1KBank(0, reg[0]);
		break;
	case 0xB002:
	case 0xB040:
		reg[0] = (reg[0] & 0x0F) | ((data & 0x0F) << 4);
		ppuMapper->setVrom1KBank(0, reg[0]);
		break;

	case 0xB001:
	case 0xB004:
	case 0xB080:
		reg[1] = (reg[1] & 0xF0) | (data & 0x0F);
		ppuMapper->setVrom1KBank(1, reg[1]);
		break;
	case 0xB003:
	case 0xB006:
	case 0xB0C0:
		reg[1] = (reg[1] & 0x0F) | ((data & 0x0F) << 4);
		ppuMapper->setVrom1KBank(1, reg[1]);
		break;

	case 0xC000:
		reg[2] = (reg[2] & 0xF0) | (data & 0x0F);
		ppuMapper->setVrom1KBank(2, reg[2]);
		break;
	case 0xC002:
	case 0xC040:
		reg[2] = (reg[2] & 0x0F) | ((data & 0x0F) << 4);
		ppuMapper->setVrom1KBank(2, reg[2]);
		break;

	case 0xC001:
	case 0xC004:
	case 0xC080:
		reg[3] = (reg[3] & 0xF0) | (data & 0x0F);
		ppuMapper->setVrom1KBank(3, reg[3]);
		break;
	case 0xC003:
	case 0xC006:
	case 0xC0C0:
		reg[3] = (reg[3] & 0x0F) | ((data & 0x0F) << 4);
		ppuMapper->setVrom1KBank(3, reg[3]);
		break;

	case 0xD000:
		reg[4] = (reg[4] & 0xF0) | (data & 0x0F);
		ppuMapper->setVrom1KBank(4, reg[4]);
		break;
	case 0xD002:
	case 0xD040:
		reg[4] = (reg[4] & 0x0F) | ((data & 0x0F) << 4);
		ppuMapper->setVrom1KBank(4, reg[4]);
		break;

	case 0xD001:
	case 0xD004:
	case 0xD080:
		reg[5] = (reg[5] & 0xF0) | (data & 0x0F);
		ppuMapper->setVrom1KBank(5, reg[5]);
		break;
	case 0xD003:
	case 0xD006:
	case 0xD0C0:
		reg[5] = (reg[5] & 0x0F) | ((data & 0x0F) << 4);
		ppuMapper->setVrom1KBank(5, reg[5]);
		break;

	case 0xE000:
		reg[6] = (reg[6] & 0xF0) | (data & 0x0F);
		ppuMapper->setVrom1KBank(6, reg[6]);
		break;
	case 0xE002:
	case 0xE040:
		reg[6] = (reg[6] & 0x0F) | ((data & 0x0F) << 4);
		ppuMapper->setVrom1KBank(6, reg[6]);
		break;

	case 0xE001:
	case 0xE004:
	case 0xE080:
		reg[7] = (reg[7] & 0xF0) | (data & 0x0F);
		ppuMapper->setVrom1KBank(7, reg[7]);
		break;
	case 0xE003:
	case 0xE006:
	case 0xE0C0:
		reg[7] = (reg[7] & 0x0F) | ((data & 0x0F) << 4);
		ppuMapper->setVrom1KBank(7, reg[7]);
		break;

	case 0xF000:
		irq_latch = (irq_latch & 0xF0) | (data & 0x0F);
		break;
	case 0xF002:
	case 0xF040:
		irq_latch = (irq_latch & 0x0F) | ((data & 0x0F) << 4);
		break;

	case 0xF003:
	case 0xF0C0:
	case 0xF006:
		irq_enable = (irq_enable & 0x01) * 3;
		irq_clock = 0;
		setIrqSignalOut(false);
		break;

	case 0xF004:
	case 0xF080:
		irq_enable = data & 0x03;
		if (irq_enable & 0x02) {
			irq_counter = irq_latch;
			irq_clock = 0;
		}
		setIrqSignalOut(false);
		break;
	}
}

void CpuMapper21::clock(uint cycles) {
	if (irq_enable & 0x02) {
		if ((irq_clock-=cycles) < 0) {
			irq_clock += 0x72;
			if (irq_counter == 0xFF) {
				irq_counter = irq_latch;
				setIrqSignalOut(true);
			} else {
				irq_counter++;
			}
		}
	}
}

bool CpuMapper21::save(QDataStream &s) {
	if (!NesCpuMapper::save(s))
		return false;
	for (int i = 0; i < 9; i++)
		s << reg[i];
	s << irq_enable;
	s << irq_counter;
	s << irq_latch;
	s << irq_clock;
	return true;
}

bool CpuMapper21::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	for (int i = 0; i < 9; i++)
		s >> reg[i];
	s >> irq_enable;
	s >> irq_counter;
	s >> irq_latch;
	s >> irq_clock;
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(21, "Konami VRC4")
