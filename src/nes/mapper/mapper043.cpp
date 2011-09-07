#include "mapper043.h"
#include <QDataStream>

void Mapper043::reset() {
	NesMapper::reset();

	setRom8KBank(3, 2);
	setRom8KBanks(1, 0, 4, 9);
	if (vromSize1KB())
		setVrom8KBank(0);
	irq_enable = 0xFF;
	irq_counter = 0;
}

quint8 Mapper043::readLow(quint16 address) {
	if (0x5000 <= address && address < 0x6000)
		return	m_rom[0x2000*8+0x1000+(address-0x5000)];
	return address >> 8;
}

void Mapper043::writeEx(quint16 address, quint8 data) {
	if ((address&0xF0FF) == 0x4022) {
		switch (data&0x07) {
		case 0x00:
		case 0x02:
		case 0x03:
		case 0x04:
			setRom8KBank(6, 4);
			break;
		case 0x01:
			setRom8KBank(6, 3);
			break;
		case 0x05:
			setRom8KBank(6, 7);
			break;
		case 0x06:
			setRom8KBank(6, 5);
			break;
		case 0x07:
			setRom8KBank(6, 6);
			break;
		}
	}
}

void Mapper043::writeLow(quint16 address, quint8 data) {
	if ((address&0xF0FF) == 0x4022)
		writeEx(address, data);
}

void Mapper043::writeHigh(quint16 address, quint8 data) {
	if (address == 0x8122) {
		if (data & 0x03) {
			irq_enable = 1;
		} else {
			irq_counter = 0;
			irq_enable = 0;
		}
		setIrqSignalOut(false);
	}
}

void Mapper043::horizontalSync(int scanline) {
	Q_UNUSED(scanline)
	setIrqSignalOut(false);
	if (irq_enable) {
		irq_counter += 341;
		if (irq_counter >= 12288) {
			irq_counter = 0;
			setIrqSignalOut(true);
		}
	}
}

#define STATE_SERIALIZE_BUILDER(sl) \
	STATE_SERIALIZE_BEGIN_##sl(Mapper043) \
	STATE_SERIALIZE_PARENT_##sl(NesMapper) \
	STATE_SERIALIZE_VAR_##sl(irq_enable) \
	STATE_SERIALIZE_VAR_##sl(irq_counter) \
	STATE_SERIALIZE_END(Mapper043)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
