#include "mapper024.h"
#include "nesppu.h"
#include "nesdisk.h"
#include <QDataStream>

void Mapper024::reset() {
	NesMapper::reset();

	irq_enable = 0;
	irq_counter = 0;
	irq_latch = 0;
	irq_clock = 0;

	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);
	if (vromSize1KB())
		setVrom8KBank(0);

	ppu()->setRenderMethod(NesPpu::PostRender);
	// TODO nes->apu->SelectExSound( 1);
}

void Mapper024::writeHigh(quint16 address, quint8 data) {
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
		setMirroring(static_cast<Mirroring>((data >> 3) & 0x03));
		break;

	case 0xC000:
		setRom8KBank(6, data);
		break;

	case 0xD000:
		setVrom1KBank(0, data);
		break;

	case 0xD001:
		setVrom1KBank(1, data);
		break;

	case 0xD002:
		setVrom1KBank(2, data);
		break;

	case 0xD003:
		setVrom1KBank(3, data);
		break;

	case 0xE000:
		setVrom1KBank(4, data);
		break;

	case 0xE001:
		setVrom1KBank(5, data);
		break;

	case 0xE002:
		setVrom1KBank(6, data);
		break;

	case 0xE003:
		setVrom1KBank(7, data);
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

void Mapper024::clock(uint cycles) {
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

#define STATE_SERIALIZE_BUILDER(sl) \
	STATE_SERIALIZE_BEGIN_##sl(Mapper024) \
	STATE_SERIALIZE_PARENT_##sl(NesMapper) \
	STATE_SERIALIZE_VAR_##sl(irq_enable) \
	STATE_SERIALIZE_VAR_##sl(irq_counter) \
	STATE_SERIALIZE_VAR_##sl(irq_latch) \
	STATE_SERIALIZE_VAR_##sl(irq_clock) \
	STATE_SERIALIZE_END(Mapper024)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
