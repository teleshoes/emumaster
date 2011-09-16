#include "mapper006.h"
#include <QDataStream>

void Mapper006::reset() {
	NesMapper::reset();

	setRom8KBanks(0, 1, 14, 15);
	if (vromSize1KB())
		setVrom8KBank(0);
	else
		setCram8KBank(0);
	irqEnable = 0;
	irqCounter = 0;
}

void Mapper006::writeLow(quint16 address, quint8 data) {
	switch (address) {
	case 0x42FE:
		if (data & 0x10)
			setMirroring(SingleHigh);
		else
			setMirroring(SingleLow);
		break;
	case 0x42FF:
		if (data & 0x10)
			setMirroring(HorizontalMirroring);
		else
			setMirroring(VerticalMirroring);
		break;
	case 0x4501:
		irqEnable = 0;
		setIrqSignalOut(false);
		break;
	case 0x4502:
		irqCounter = (irqCounter&0xFF00) | (data<<0);
		break;
	case 0x4503:
		irqCounter = (irqCounter&0x00FF) | (data<<8);
		irqEnable = 0xFF;
		setIrqSignalOut(false);
		break;
	default:
		Mapper006::writeLow(address, data);
		break;
	}
}

void Mapper006::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(address)
	setRom16KBank(4, (data & 0x3C) >> 2);
	setCram8KBank(data & 0x03);
}

void Mapper006::horizontalSync(int scanline) {
	Q_UNUSED(scanline)
	if(irqEnable) {
		irqCounter += 133;
		if (irqCounter >= 0xFFFF) {
			irqCounter = 0;
			setIrqSignalOut(true);
		}
	}
}

#define STATE_SERIALIZE_BUILDER(sl) \
STATE_SERIALIZE_BEGIN_##sl(Mapper006, 1) \
	STATE_SERIALIZE_PARENT_##sl(NesMapper) \
	STATE_SERIALIZE_VAR_##sl(irqEnable) \
	STATE_SERIALIZE_VAR_##sl(irqCounter) \
STATE_SERIALIZE_END_##sl(Mapper006)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
