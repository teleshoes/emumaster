#include "mapper232.h"
#include "nesppu.h"
#include "nesdisk.h"
#include <QDataStream>

void Mapper232::reset() {
	NesMapper::reset();

	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);

	reg[0] = 0x0C;
	reg[1] = 0x00;
}

void Mapper232::writeLow(quint16 address, quint8 data) {
	if (address >= 0x6000)
		write(address, data);
}

void Mapper232::writeHigh(quint16 address, quint8 data) {
	if (address <= 0x9FFF)
		reg[0] = (data & 0x18)>>1;
	else
		reg[1] = data & 0x03;

	setRom8KBank(4, (reg[0]|reg[1])*2+0);
	setRom8KBank(5, (reg[0]|reg[1])*2+1);
	setRom8KBank(6, (reg[0]|0x03)*2+0);
	setRom8KBank(7, (reg[0]|0x03)*2+1);
}

#define STATE_SERIALIZE_BUILDER(sl) \
	STATE_SERIALIZE_BEGIN_##sl(Mapper232) \
	STATE_SERIALIZE_PARENT_##sl(NesMapper) \
	STATE_SERIALIZE_ARRAY_##sl(reg, sizeof(reg)) \
	STATE_SERIALIZE_END(Mapper232)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)