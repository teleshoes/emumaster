#include "mapper048.h"
#include "ppu.h"
#include <QDataStream>

void Mapper048::reset() {
	NesMapper::reset();

	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);
	if (vromSize1KB())
		setVrom8KBank(0);

	reg = 0;
	irq_enable = 0;
	irq_counter = 0;
}

void Mapper048::writeHigh(quint16 address, quint8 data) {
	switch (address) {
	case 0x8000:
		if (!reg)
			setMirroring(static_cast<Mirroring>((data & 0x40) >> 6));
		setRom8KBank(4, data);
		break;
	case 0x8001:
		setRom8KBank(5, data);
		break;

	case 0x8002:
		setVrom2KBank(0, data);
		break;
	case 0x8003:
		setVrom2KBank(2, data);
		break;
	case 0xA000:
		setVrom1KBank(4, data);
		break;
	case 0xA001:
		setVrom1KBank(5, data);
		break;
	case 0xA002:
		setVrom1KBank(6, data);
		break;
	case 0xA003:
		setVrom1KBank(7, data);
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
		setMirroring(static_cast<Mirroring>((data & 0x40) >> 6));
		reg = 1;
		break;
	}
}

void Mapper048::horizontalSync(int scanline) {
	if (scanline < NesPpu::VisibleScreenHeight && ppuRegisters()->isDisplayOn()) {
		if (irq_enable) {
			if (irq_counter == 0xFF)
				setIrqSignalOut(true); // TODO need to be cleared somewhere (trigger in virtuanes)
			irq_counter++;
		}
	}
}

#define STATE_SERIALIZE_BUILDER(sl) \
STATE_SERIALIZE_BEGIN_##sl(Mapper048, 1) \
	STATE_SERIALIZE_PARENT_##sl(NesMapper) \
	STATE_SERIALIZE_VAR_##sl(reg) \
	STATE_SERIALIZE_VAR_##sl(irq_enable) \
	STATE_SERIALIZE_VAR_##sl(irq_counter) \
STATE_SERIALIZE_END_##sl(Mapper048)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
