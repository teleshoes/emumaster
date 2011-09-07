#include "mapper042.h"
#include <QDataStream>

void Mapper042::reset() {
	NesMapper::reset();

	setRom8KBank(3, 0);
	setRom8KBanks(romSize8KB()-4, romSize8KB()-3, romSize8KB()-2, romSize8KB()-1);
	if (vromSize1KB())
		setVrom8KBank(0);
	irq_enable = 0;
	irq_counter = 0;
}

void Mapper042::writeHigh(quint16 address, quint8 data) {
	switch (address & 0xE003) {
	case 0xE000:
		setRom8KBank(3, data&0x0F);
		break;

	case 0xE001:
		setMirroring(static_cast<Mirroring>((data & 0x08) >> 3));
		break;

	case 0xE002:
		if (data&0x02) {
			irq_enable = 0xFF;
		} else {
			irq_enable = 0;
			irq_counter = 0;
		}
		setIrqSignalOut(false);
		break;
	}
}

void Mapper042::horizontalSync(int scanline) {
	Q_UNUSED(scanline)
	setIrqSignalOut(false);
	if (irq_enable) {
		if (irq_counter < 215) {
			irq_counter++;
		}
		if (irq_counter == 215) {
			irq_enable = 0;
			setIrqSignalOut(true);
		}
	}
}

#define STATE_SERIALIZE_BUILDER(sl) \
	STATE_SERIALIZE_BEGIN_##sl(Mapper042) \
	STATE_SERIALIZE_PARENT_##sl(NesMapper) \
	STATE_SERIALIZE_VAR_##sl(irq_enable) \
	STATE_SERIALIZE_VAR_##sl(irq_counter) \
	STATE_SERIALIZE_END(Mapper042)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
