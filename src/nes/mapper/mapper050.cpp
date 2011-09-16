#include "mapper050.h"
#include <QDataStream>

void Mapper050::reset() {
	NesMapper::reset();

	irq_enable = 0;
	setRom8KBank(3, 15);
	setRom8KBank(4, 8);
	setRom8KBank(5, 9);
	setRom8KBank(6, 0);
	setRom8KBank(7, 11);
	if (vromSize1KB())
		setVrom8KBank(0);
}

void Mapper050::writeEx(quint16 address, quint8 data) {
	if ((address & 0xE060) == 0x4020) {
		if (address & 0x0100) {
			irq_enable = data & 0x01;
			setIrqSignalOut(false);
		} else {
			setRom8KBank(6, (data&0x08)|((data&0x01)<<2)|((data&0x06)>>1));
		}
	}
}

void Mapper050::writeLow(quint16 address, quint8 data) {
	if ((address & 0xE060) == 0x4020) {
		if (address & 0x0100) {
			irq_enable = data & 0x01;
			setIrqSignalOut(false);
		} else {
			setRom8KBank(6, (data&0x08)|((data&0x01)<<2)|((data&0x06)>>1));
		}
	}
}

void Mapper050::horizontalSync(int scanline) {
	if (irq_enable) {
		if (scanline == 21)
			setIrqSignalOut(true);
	}
}

#define STATE_SERIALIZE_BUILDER(sl) \
STATE_SERIALIZE_BEGIN_##sl(Mapper050, 1) \
	STATE_SERIALIZE_PARENT_##sl(NesMapper) \
	STATE_SERIALIZE_VAR_##sl(irq_enable) \
STATE_SERIALIZE_END_##sl(Mapper050)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
