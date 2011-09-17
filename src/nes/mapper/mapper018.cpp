#include "mapper018.h"
#include "ppu.h"
#include "disk.h"
#include <QDataStream>

void Mapper018::reset() {
	NesMapper::reset();

	for (int i = 0; i < 11; i++)
		reg[i] = 0;
	reg[2] = nesRomSize8KB-2;
	reg[3] = nesRomSize8KB-1;

	setRom8KBanks(0, 1, nesRomSize8KB-2, nesRomSize8KB-1);

	irq_enable  = 0;
	irq_mode    = 0;
	irq_counter = 0xFFFF;
	irq_latch   = 0xFFFF;

	u32 crc = nesDiskCrc;

	if (crc == 0xefb1df9e) {	// The Lord of King(J)
		nesPpu.setRenderMethod(NesPpu::PreAllRender);
	}
	if (crc == 0x3746f951) {	// Pizza Pop!(J)
		nesPpu.setRenderMethod(NesPpu::PreAllRender);
	}
}

void Mapper018::writeHigh(u16 address, u8 data) {
	switch (address) {
	case 0x8000:
		reg[0] = (reg[0] & 0xF0) | (data & 0x0F);
		setRom8KBank(4, reg[0]);
		break;
	case 0x8001:
		reg[0] = (reg[0] & 0x0F) | ((data & 0x0F) << 4);
		setRom8KBank(4, reg[0]);
		break;
	case 0x8002:
		reg[1] = (reg[1] & 0xF0) | (data & 0x0F);
		setRom8KBank(5, reg[1]);
		break;
	case 0x8003:
		reg[1] = (reg[1] & 0x0F) | ((data & 0x0F) << 4);
		setRom8KBank(5, reg[1]);
		break;
	case 0x9000:
		reg[2] = (reg[2] & 0xF0) | (data & 0x0F);
		setRom8KBank(6, reg[2]);
		break;
	case 0x9001:
		reg[2] = (reg[2] & 0x0F) | ((data & 0x0F) << 4);
		setRom8KBank(6, reg[2]);
		break;

	case 0xA000:
		reg[3] = (reg[3] & 0xF0) | (data & 0x0F);
		setVrom1KBank(0, reg[3]);
		break;
	case 0xA001:
		reg[3] = (reg[3] & 0x0F) | ((data & 0x0F) << 4);
		setVrom1KBank(0, reg[3]);
		break;
	case 0xA002:
		reg[4] = (reg[4] & 0xF0) | (data & 0x0F);
		setVrom1KBank(1, reg[4]);
		break;
	case 0xA003:
		reg[4] = (reg[4] & 0x0F) | ((data & 0x0F) << 4);
		setVrom1KBank(1, reg[4]);
		break;

	case 0xB000:
		reg[5] = (reg[5] & 0xF0) | (data & 0x0F);
		setVrom1KBank(2, reg[5]);
		break;
	case 0xB001:
		reg[5] = (reg[5] & 0x0F) | ((data & 0x0F) << 4);
		setVrom1KBank(2, reg[5]);
		break;
	case 0xB002:
		reg[6] = (reg[6] & 0xF0) | (data & 0x0F);
		setVrom1KBank(3, reg[6]);
		break;
	case 0xB003:
		reg[6] = (reg[6] & 0x0F) | ((data & 0x0F) << 4);
		setVrom1KBank(3, reg[6]);
		break;

	case 0xC000:
		reg[7] = (reg[7] & 0xF0) | (data & 0x0F);
		setVrom1KBank(4, reg[7]);
		break;
	case 0xC001:
		reg[7] = (reg[7] & 0x0F) | ((data & 0x0F) << 4);
		setVrom1KBank(4, reg[7]);
		break;
	case 0xC002:
		reg[8] = (reg[8] & 0xF0) | (data & 0x0F);
		setVrom1KBank(5, reg[8]);
		break;
	case 0xC003:
		reg[8] = (reg[8] & 0x0F) | ((data & 0x0F) << 4);
		setVrom1KBank(5, reg[8]);
		break;

	case 0xD000:
		reg[9] = (reg[9] & 0xF0) | (data & 0x0F);
		setVrom1KBank(6, reg[9]);
		break;
	case 0xD001:
		reg[9] = (reg[9] & 0x0F) | ((data & 0x0F) << 4);
		setVrom1KBank(6, reg[9]);
		break;
	case 0xD002:
		reg[10] = (reg[10] & 0xF0) | (data & 0x0F);
		setVrom1KBank(7, reg[10]);
		break;
	case 0xD003:
		reg[10] = (reg[10] & 0x0F) | ((data & 0x0F) << 4);
		setVrom1KBank(7, reg[10]);
		break;

	case 0xE000:
		irq_latch = (irq_latch & 0xFFF0) | (data & 0x0F);
		break;
	case 0xE001:
		irq_latch = (irq_latch & 0xFF0F) | ((data & 0x0F) << 4);
		break;
	case 0xE002:
		irq_latch = (irq_latch & 0xF0FF) | ((data & 0x0F) << 8);
		break;
	case 0xE003:
		irq_latch = (irq_latch & 0x0FFF) | ((data & 0x0F) << 12);
		break;

	case 0xF000:
		irq_counter = irq_latch;
		break;
	case 0xF001:
		irq_mode = (data>>1) & 0x07;
		irq_enable = (data & 0x01);
		setIrqSignalOut(false);
		break;

	case 0xF002:
		data &= 0x03;
		if (data == 0)
			setMirroring(HorizontalMirroring);
		else if (data == 1)
			setMirroring(VerticalMirroring);
		else
			setMirroring(SingleLow);
		break;
	}
}

void Mapper018::clock(uint cycles) {
	bool bIRQ = false;
	int	irq_counter_old = irq_counter;

	if (irq_enable && irq_counter) {
		irq_counter -= cycles;

		switch( irq_mode) {
		case 0:
			if (irq_counter <= 0) {
				bIRQ = TRUE;
			}
			break;
		case 1:
			if ((irq_counter & 0xF000) != (irq_counter_old & 0xF000)) {
				bIRQ = TRUE;
			}
			break;
		case 2:
		case 3:
			if ((irq_counter & 0xFF00) != (irq_counter_old & 0xFF00)) {
				bIRQ = TRUE;
			}
			break;
		case 4:
		case 5:
		case 6:
		case 7:
			if ((irq_counter & 0xFFF0) != (irq_counter_old & 0xFFF0)) {
				bIRQ = TRUE;
			}
			break;
		}

		if (bIRQ) {
			irq_counter = 0;
			irq_enable = 0;
			setIrqSignalOut(true);
		}
	}
}

#define STATE_SERIALIZE_BUILDER(sl) \
STATE_SERIALIZE_BEGIN_##sl(Mapper018, 1) \
	STATE_SERIALIZE_PARENT_##sl(NesMapper) \
	STATE_SERIALIZE_ARRAY_##sl(reg, sizeof(reg)) \
	STATE_SERIALIZE_VAR_##sl(irq_enable) \
	STATE_SERIALIZE_VAR_##sl(irq_mode) \
	STATE_SERIALIZE_VAR_##sl(irq_counter) \
	STATE_SERIALIZE_VAR_##sl(irq_latch) \
STATE_SERIALIZE_END_##sl(Mapper018)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
