#include "mapper065.h"
#include "nesdisk.h"
#include "nesppu.h"
#include <QDataStream>

void Mapper065::reset() {
	NesMapper::reset();

	patch = 0;

	// Kaiketsu Yanchamaru 3(J)
	if (disk()->crc() == 0xe30b7f64)
		patch = 1;

	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);

	if (vromSize1KB())
		setVrom8KBank(0);

	irq_enable = 0;
	irq_counter = 0;
}

void Mapper065::writeHigh(quint16 address, quint8 data) {
	switch (address) {
	case 0x8000:
		setRom8KBank(4, data);
		break;

	case 0x9000:
		if (!patch)
			setMirroring(static_cast<Mirroring>((data & 0x40) >> 6));
		break;

	case 0x9001:
		if (patch)
			setMirroring(static_cast<Mirroring>((data & 0x80) >> 7));
		break;

	case 0x9003:
		if (!patch) {
			irq_enable = data & 0x80;
			setIrqSignalOut(false);
		}
		break;
	case 0x9004:
		if (!patch) {
			irq_counter = irq_latch;
		}
		break;
	case 0x9005:
		if (patch) {
			irq_counter = data<<1;
			irq_enable = data;
			setIrqSignalOut(false);
		} else {
			irq_latch = (irq_latch & 0x00FF)|(data<<8);
		}
		break;

	case 0x9006:
		if (patch) {
			irq_enable = 1;
		} else {
			irq_latch = (irq_latch & 0xFF00)|data;
		}
		break;

	case 0xB000:
	case 0xB001:
	case 0xB002:
	case 0xB003:
	case 0xB004:
	case 0xB005:
	case 0xB006:
	case 0xB007:
		setVrom1KBank(address & 0x0007, data);
		break;

	case 0xA000:
		setRom8KBank(5, data);
		break;
	case 0xC000:
		setRom8KBank(6, data);
		break;
	}
}

void Mapper065::clock(uint cycles) {
	if (!patch) {
		if (irq_enable) {
			if (irq_counter <= 0) {
				setIrqSignalOut(true);
			} else {
				irq_counter -= cycles;
			}
		}
	}
}

void Mapper065::horizontalSync(int scanline) {
	Q_UNUSED(scanline)
	if (patch) {
		if (irq_enable) {
			if (irq_counter == 0) {
				setIrqSignalOut(true);
			} else {
				irq_counter--;
			}
		}
	}
}

#define STATE_SERIALIZE_BUILDER(sl) \
	STATE_SERIALIZE_BEGIN_##sl(Mapper065) \
	STATE_SERIALIZE_PARENT_##sl(NesMapper) \
	STATE_SERIALIZE_VAR_##sl(irq_enable) \
	STATE_SERIALIZE_VAR_##sl(irq_counter) \
	STATE_SERIALIZE_VAR_##sl(irq_latch) \
	STATE_SERIALIZE_END(Mapper065)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
