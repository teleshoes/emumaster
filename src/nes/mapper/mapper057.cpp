#include "mapper057.h"
#include <QDataStream>

void Mapper057::reset() {
	NesMapper::reset();

	setRom8KBanks(0, 1, 0, 1);
	setVrom8KBank(0);

	reg = 0;
}

void Mapper057::writeHigh(quint16 address, quint8 data) {
	switch (address) {
	case 0x8000:
	case 0x8001:
	case 0x8002:
	case 0x8003:
		if (data & 0x40)
			setVrom8KBank((data&0x03) + ((reg&0x10)>>1) + (reg&0x07));
		break;
	case 0x8800:
		reg = data;
		if (data & 0x80) {
			setRom32KBank(((data & 0x40) >> 6) + 2);
		} else {
			setRom16KBank(4, (data & 0x60) >> 5);
			setRom16KBank(6, (data & 0x60) >> 5);
		}
		setVrom8KBank((data&0x07) + ((data&0x10)>>1));
		setMirroring(static_cast<Mirroring>((data & 0x08) >> 3));
		break;
	}
}

#define STATE_SERIALIZE_BUILDER(sl) \
	STATE_SERIALIZE_BEGIN_##sl(Mapper057) \
	STATE_SERIALIZE_PARENT_##sl(NesMapper) \
	STATE_SERIALIZE_VAR_##sl(reg) \
	STATE_SERIALIZE_END(Mapper057)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
