#include "mapper046.h"
#include <QDataStream>

void Mapper046::reset() {
	NesMapper::reset();
	qMemSet(reg, 0, 4);
	updateBanks();
	setMirroring(Vertical);
}

void Mapper046::writeLow(quint16 address, quint8 data) {
	Q_UNUSED(address)
	reg[0] = data & 0x0F;
	reg[1] = (data & 0xF0) >> 4;
	updateBanks();
}

void Mapper046::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(address)
	reg[2] = data & 0x01;
	reg[3] = (data & 0x70) >> 4;
	updateBanks();
}

void Mapper046::updateBanks() {
	setRom32KBank(reg[0]*2+reg[2]);
	setVrom8KBank(reg[1]*8+reg[3]);
}

#define STATE_SERIALIZE_BUILDER(sl) \
	STATE_SERIALIZE_BEGIN_##sl(Mapper046) \
	STATE_SERIALIZE_PARENT_##sl(NesMapper) \
	STATE_SERIALIZE_ARRAY_##sl(reg, sizeof(reg)) \
	STATE_SERIALIZE_END(Mapper046)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
