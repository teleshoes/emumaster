#include "mapper041.h"
#include <QDataStream>

void Mapper041::reset() {
	NesMapper::reset();

	reg[0] = reg[1] = 0;
	setRom32KBank(0);
	if (nesVromSize1KB)
		setVrom8KBank(0);
}

void Mapper041::writeLow(u16 address, u8 data) {
	Q_UNUSED(data)
	if (address >= 0x6000 && address < 0x6800) {
		setRom32KBank(address & 0x07);
		reg[0] = address & 0x04;
		reg[1] &= 0x03;
		reg[1] |= (address>>1) & 0x0C;
		setVrom8KBank(reg[1]);
		setMirroring(static_cast<NesMirroring>((data & 0x20) >> 5));
	}
}

void Mapper041::writeHigh(u16 address, u8 data) {
	Q_UNUSED(data)
	if (reg[0]) {
		reg[1] &= 0x0C;
		reg[1] &= address & 0x03;
		setVrom8KBank(reg[1]);
	}
}

#define STATE_SERIALIZE_BUILDER(sl) \
STATE_SERIALIZE_BEGIN_##sl(Mapper041, 1) \
	STATE_SERIALIZE_PARENT_##sl(NesMapper) \
	STATE_SERIALIZE_ARRAY_##sl(reg, sizeof(reg)) \
STATE_SERIALIZE_END_##sl(Mapper041)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
