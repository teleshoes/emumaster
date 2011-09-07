#include "mapper027.h"
#include "nesppu.h"
#include "nesdisk.h"
#include <QDataStream>

void Mapper027::reset() {
	NesMapper::reset();

	for (int i = 0; i < 8; i++)
		reg[i] = i;
	reg[8] = 0;

	irq_enable = 0;
	irq_counter = 0;
	irq_latch = 0;
	irq_clock = 0;

	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);

	quint32 crc = disk()->crc();
	if( crc == 0x47DCBCC4 ) {	// Gradius II(sample)
		ppu()->setRenderMethod(NesPpu::PostRender);
	}
	if( crc == 0x468F21FC ) {	// Racer Mini 4 ku(sample)
		ppu()->setRenderMethod(NesPpu::PostRender);
	}
}

void Mapper027::writeHigh(quint16 address, quint8 data) {
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
		setMirroring(static_cast<Mirroring>(data & 3));
		break;

	case 0x9002:
	case 0x9080:
		reg[8] = data;
		break;

	case 0xB000:
		reg[0] = (reg[0] & 0xF0) | (data & 0x0F);
		setVrom1KBank(0, reg[0] );
		break;
	case 0xB001:
		reg[0] = (reg[0] & 0x0F) | (data<< 4);
		setVrom1KBank(0, reg[0] );
		break;

	case 0xB002:
		reg[1] = (reg[1] & 0xF0) | (data & 0x0F);
		setVrom1KBank(1, reg[1] );
		break;
	case 0xB003:
		reg[1] = (reg[1] & 0x0F) | (data<< 4);
		setVrom1KBank(1, reg[1] );
		break;

	case 0xC000:
		reg[2] = (reg[2] & 0xF0) | (data & 0x0F);
		setVrom1KBank(2, reg[2] );
		break;
	case 0xC001:
		reg[2] = (reg[2] & 0x0F) | (data<< 4);
		setVrom1KBank(2, reg[2] );
		break;

	case 0xC002:
		reg[3] = (reg[3] & 0xF0) | (data & 0x0F);
		setVrom1KBank(3, reg[3] );
		break;
	case 0xC003:
		reg[3] = (reg[3] & 0x0F) | (data<< 4);
		setVrom1KBank(3, reg[3] );
		break;

	case 0xD000:
		reg[4] = (reg[4] & 0xF0) | (data & 0x0F);
		setVrom1KBank(4, reg[4] );
		break;
	case 0xD001:
		reg[4] = (reg[4] & 0x0F) | (data<< 4);
		setVrom1KBank(4, reg[4] );
		break;

	case 0xD002:
		reg[5] = (reg[5] & 0xF0) | (data & 0x0F);
		setVrom1KBank(5, reg[5] );
		break;
	case 0xD003:
		reg[5] = (reg[5] & 0x0F) | (data << 4);
		setVrom1KBank(5, reg[5] );
		break;

	case 0xE000:
		reg[6] = (reg[6] & 0xF0) | (data & 0x0F);
		setVrom1KBank(6, reg[6] );
		break;
	case 0xE001:
		reg[6] = (reg[6] & 0x0F) | (data << 4);
		setVrom1KBank(6, reg[6] );
		break;

	case 0xE002:
		reg[7] = (reg[7] & 0xF0) | (data & 0x0F);
		setVrom1KBank(7, reg[7] );
		break;
	case 0xE003:
		reg[7] = (reg[7] & 0x0F) | (data<< 4);
		setVrom1KBank(7, reg[7] );
		break;

	case 0xF000:
		irq_latch = (irq_latch & 0xF0) | (data & 0x0F);
		setIrqSignalOut(false);
		break;
	case 0xF001:
		irq_latch = (irq_latch & 0x0F) | ((data & 0x0F) << 4);
		setIrqSignalOut(false);
		break;

	case 0xF003:
		irq_enable = (irq_enable & 0x01) * 3;
		irq_clock = 0;
		setIrqSignalOut(false);
		break;

	case 0xF002:
		irq_enable = data & 0x03;
		if( irq_enable & 0x02 ) {
			irq_counter = irq_latch;
			irq_clock = 0;
		}
		setIrqSignalOut(false);
		break;
	}
}

void Mapper027::horizontalSync(int scanline) {
	Q_UNUSED(scanline)
	if (irq_enable & 0x02) {
		if (irq_counter == 0xFF) {
			irq_counter = irq_latch;
			setIrqSignalOut(true);
		} else {
			irq_counter++;
		}
	}
}

#define STATE_SERIALIZE_BUILDER(sl) \
	STATE_SERIALIZE_BEGIN_##sl(Mapper027) \
	STATE_SERIALIZE_PARENT_##sl(NesMapper) \
	STATE_SERIALIZE_ARRAY_##sl(reg, sizeof(reg)) \
	STATE_SERIALIZE_VAR_##sl(irq_enable) \
	STATE_SERIALIZE_VAR_##sl(irq_counter) \
	STATE_SERIALIZE_VAR_##sl(irq_latch) \
	STATE_SERIALIZE_VAR_##sl(irq_clock) \
	STATE_SERIALIZE_END(Mapper027)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
