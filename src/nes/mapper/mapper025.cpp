#include "mapper025.h"
#include "ppu.h"
#include "disk.h"
#include <QDataStream>

void Mapper025::reset() {
	NesMapper::reset();

	for (int i = 0; i < 11; i++)
		reg[i] = 0;
	reg[9] = romSize8KB()-2;

	irq_enable = 0;
	irq_counter = 0;
	irq_latch = 0;
	irq_clock = 0;
	irq_occur = 0;

	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);
	if (vromSize1KB())
		setVrom8KBank(0);

	quint32 crc = disk()->crc();
	if (crc == 0xa2e68da8) {	// For Racer Mini Yonku - Japan Cup(J)
		ppu()->setRenderMethod(NesPpu::TileRender);
	}
	if (crc == 0xea74c587) {	// For Teenage Mutant Ninja Turtles(J)
		ppu()->setRenderMethod(NesPpu::TileRender);
	}
	if (crc == 0x0bbd85ff) {	// For Bio Miracle Bokutte Upa(J)
		ppu()->setRenderMethod(NesPpu::PreAllRender);
	}
}

void Mapper025::writeHigh(quint16 address, quint8 data) {
	switch (address & 0xF000) {
	case 0x8000:
		if (reg[10] & 0x02) {
			reg[9] = data;
			setRom8KBank(6, data);
		} else {
			reg[8] = data;
			setRom8KBank(4, data);
		}
		break;
	case 0xA000:
		setRom8KBank(5, data);
		break;
	}

	switch (address & 0xF00F) {
	case 0x9000:
		setMirroring(static_cast<Mirroring>(data & 3));
		break;

	case 0x9001:
	case 0x9004:
		if((reg[10] & 0x02) != (data & 0x02)) {
			quint8 swap = reg[8];
			reg[8] = reg[9];
			reg[9] = swap;

			setRom8KBank(4, reg[8]);
			setRom8KBank(6, reg[9]);
		}
		reg[10] = data;
		break;

	case 0xB000:
		reg[0] = (reg[0] & 0xF0) | (data & 0x0F);
		setVrom1KBank(0, reg[0]);
		break;
	case 0xB002:
	case 0xB008:
		reg[0] = (reg[0] & 0x0F) | ((data & 0x0F) << 4);
		setVrom1KBank(0, reg[0]);
		break;

	case 0xB001:
	case 0xB004:
		reg[1] = (reg[1] & 0xF0) | (data & 0x0F);
		setVrom1KBank(1, reg[1]);
		break;
	case 0xB003:
	case 0xB00C:
		reg[1] = (reg[1] & 0x0F) | ((data & 0x0F) << 4);
		setVrom1KBank(1, reg[1]);
		break;

	case 0xC000:
		reg[2] = (reg[2] & 0xF0) | (data & 0x0F);
		setVrom1KBank(2, reg[2]);
		break;
	case 0xC002:
	case 0xC008:
		reg[2] = (reg[2] & 0x0F) | ((data & 0x0F) << 4);
		setVrom1KBank(2, reg[2]);
		break;

	case 0xC001:
	case 0xC004:
		reg[3] = (reg[3] & 0xF0) | (data & 0x0F);
		setVrom1KBank(3, reg[3]);
		break;
	case 0xC003:
	case 0xC00C:
		reg[3] = (reg[3] & 0x0F) | ((data & 0x0F) << 4);
		setVrom1KBank(3, reg[3]);
		break;

	case 0xD000:
		reg[4] = (reg[4] & 0xF0) | (data & 0x0F);
		setVrom1KBank(4, reg[4]);
		break;
	case 0xD002:
	case 0xD008:
		reg[4] = (reg[4] & 0x0F) | ((data & 0x0F) << 4);
		setVrom1KBank(4, reg[4]);
		break;

	case 0xD001:
	case 0xD004:
		reg[5] = (reg[5] & 0xF0) | (data & 0x0F);
		setVrom1KBank(5, reg[5]);
		break;
	case 0xD003:
	case 0xD00C:
		reg[5] = (reg[5] & 0x0F) | ((data & 0x0F) << 4);
		setVrom1KBank(5, reg[5]);
		break;

	case 0xE000:
		reg[6] = (reg[6] & 0xF0) | (data & 0x0F);
		setVrom1KBank(6, reg[6]);
		break;
	case 0xE002:
	case 0xE008:
		reg[6] = (reg[6] & 0x0F) | ((data & 0x0F) << 4);
		setVrom1KBank(6, reg[6]);
		break;

	case 0xE001:
	case 0xE004:
		reg[7] = (reg[7] & 0xF0) | (data & 0x0F);
		setVrom1KBank(7, reg[7]);
		break;
	case 0xE003:
	case 0xE00C:
		reg[7] = (reg[7] & 0x0F) | ((data & 0x0F) << 4);
		setVrom1KBank(7, reg[7]);
		break;

	case 0xF000:
		irq_latch = (irq_latch & 0xF0) | (data & 0x0F);
		setIrqSignalOut(false);
		break;

	case 0xF002:
	case 0xF008:
		irq_latch = (irq_latch & 0x0F) | ((data & 0x0F) << 4);
		setIrqSignalOut(false);
		break;

	case 0xF001:
	case 0xF004:
		irq_enable = data & 0x03;
		irq_counter = irq_latch;
		irq_clock = 0;
		setIrqSignalOut(false);
		break;

	case 0xF003:
	case 0xF00C:
		irq_enable = (irq_enable & 0x01)*3;
		setIrqSignalOut(false);
		break;
	}
}

void Mapper025::clock(uint cycles) {
	if (irq_enable & 0x02) {
		irq_clock += cycles*3;
		while (irq_clock >= 341) {
			irq_clock -= 341;
			irq_counter++;
			if (irq_counter == 0) {
				irq_counter = irq_latch;
				setIrqSignalOut(true);
			}
		}
	}
}

#define STATE_SERIALIZE_BUILDER(sl) \
STATE_SERIALIZE_BEGIN_##sl(Mapper025, 1) \
	STATE_SERIALIZE_PARENT_##sl(NesMapper) \
	STATE_SERIALIZE_ARRAY_##sl(reg, sizeof(reg)) \
	STATE_SERIALIZE_VAR_##sl(irq_enable) \
	STATE_SERIALIZE_VAR_##sl(irq_counter) \
	STATE_SERIALIZE_VAR_##sl(irq_latch) \
	STATE_SERIALIZE_VAR_##sl(irq_clock) \
	STATE_SERIALIZE_VAR_##sl(irq_occur) \
STATE_SERIALIZE_END_##sl(Mapper025)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
