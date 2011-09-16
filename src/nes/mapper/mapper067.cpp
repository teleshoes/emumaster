#include "mapper067.h"
#include "disk.h"
#include "ppu.h"
#include <QDataStream>

void Mapper067::reset() {
	NesMapper::reset();

	irq_enable = 0;
	irq_toggle = 0;
	irq_counter = 0;

	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);

	setVrom4KBank(0, 0);
	setVrom4KBank(4, vromSize4KB()-1);

	quint32 crc = disk()->crc();

	if (crc == 0x7f2a04bf) // For Fantasy Zone 2(J)
		ppu()->setRenderMethod(NesPpu::PreAllRender);
}

void Mapper067::writeHigh(quint16 address, quint8 data) {
	switch (address & 0xF800) {
	case 0x8800:
		setVrom2KBank(0, data);
		break;
	case 0x9800:
		setVrom2KBank(2, data);
		break;
	case 0xA800:
		setVrom2KBank(4, data);
		break;
	case 0xB800:
		setVrom2KBank(6, data);
		break;

	case 0xC800:
		if (!irq_toggle) {
			irq_counter = (irq_counter&0x00FF) | (data<<8);
		} else {
			irq_counter = (irq_counter&0xFF00) | data;
		}
		irq_toggle ^= 1;
		setIrqSignalOut(false);
		break;
	case 0xD800:
		irq_enable = data & 0x10;
		irq_toggle = 0;
		setIrqSignalOut(false);
		break;

	case 0xE800:
		setMirroring(static_cast<Mirroring>(data & 0x03));
		break;

	case 0xF800:
		setRom16KBank(4, data);
		break;
	}
}

void Mapper067::clock(uint cycles) {
	if (irq_enable) {
		if ((irq_counter -= cycles) <= 0) {
			irq_enable = 0;
			irq_counter = 0xFFFF;
			setIrqSignalOut(true);
		}
	}
}

#define STATE_SERIALIZE_BUILDER(sl) \
STATE_SERIALIZE_BEGIN_##sl(Mapper067, 1) \
	STATE_SERIALIZE_PARENT_##sl(NesMapper) \
	STATE_SERIALIZE_VAR_##sl(irq_enable) \
	STATE_SERIALIZE_VAR_##sl(irq_counter) \
	STATE_SERIALIZE_VAR_##sl(irq_toggle) \
STATE_SERIALIZE_END_##sl(Mapper067)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
