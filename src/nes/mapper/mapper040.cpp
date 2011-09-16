#include "mapper040.h"
#include <QDataStream>

void Mapper040::reset() {
	NesMapper::reset();

	setRom8KBank(3, 6);
	setRom8KBanks(4, 5, 0, 7);
	if (vromSize1KB())
		setVrom8KBank(0);
	irq_enable = 0;
	irq_line = 0;
}

void Mapper040::writeHigh(quint16 address, quint8 data) {
	switch (address & 0xE000) {
	case 0x8000:
		irq_enable = 0;
		setIrqSignalOut(false);
		break;
	case 0xA000:
		irq_enable = 0xFF;
		irq_line = 37;
		setIrqSignalOut(false);
		break;
	case 0xC000:
		break;
	case 0xE000:
		setRom8KBank(6, data&0x07);
		break;
	}
}

void Mapper040::horizontalSync(int scanline) {
	Q_UNUSED(scanline)
	if (irq_enable) {
		if (--irq_line <= 0)
			setIrqSignalOut(true);
	}
}

#define STATE_SERIALIZE_BUILDER(sl) \
STATE_SERIALIZE_BEGIN_##sl(Mapper040, 1) \
	STATE_SERIALIZE_PARENT_##sl(NesMapper) \
	STATE_SERIALIZE_VAR_##sl(irq_enable) \
	STATE_SERIALIZE_VAR_##sl(irq_line) \
STATE_SERIALIZE_END_##sl(Mapper040)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
