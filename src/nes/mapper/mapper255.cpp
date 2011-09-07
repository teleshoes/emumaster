#include "mapper255.h"
#include "nesppu.h"
#include "nesdisk.h"
#include <imachine.h>
#include <QDataStream>

void Mapper255::reset() {
	NesMapper::reset();

	setRom32KBank(0);
	setVrom8KBank(0);
	setMirroring(Vertical);

	for (int i = 0; i < 4; i++)
		reg[i] = 0;
}

quint8 Mapper255::readLow(quint16 address) {
	if (address >= 0x5800)
		return reg[address & 0x0003] & 0x0F;
	else
		return address >> 8;
}

void Mapper255::writeLow(quint16 address, quint8 data) {
	if (address >= 0x5800)
		reg[address & 0x0003] = data & 0x0F;
}

void Mapper255::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(data)

	quint8 prg = (address & 0x0F80) >> 7;
	int chr = (address & 0x003F);
	int bank = (address & 0x4000) >> 14;

	if (address & 0x2000)
		setMirroring(Horizontal);
	else
		setMirroring(Vertical);

	if (address & 0x1000) {
		if (address & 0x0040) {
			setRom8KBank(4, 0x80*bank+prg*4+2);
			setRom8KBank(5, 0x80*bank+prg*4+3);
			setRom8KBank(6, 0x80*bank+prg*4+2);
			setRom8KBank(7, 0x80*bank+prg*4+3);
		} else {
			setRom8KBank(4, 0x80*bank+prg*4+0);
			setRom8KBank(5, 0x80*bank+prg*4+1);
			setRom8KBank(6, 0x80*bank+prg*4+0);
			setRom8KBank(7, 0x80*bank+prg*4+1);
		}
	} else {
		setRom8KBank(4, 0x80*bank+prg*4+0);
		setRom8KBank(5, 0x80*bank+prg*4+1);
		setRom8KBank(6, 0x80*bank+prg*4+2);
		setRom8KBank(7, 0x80*bank+prg*4+3);
	}

	setVrom1KBank(0, 0x200*bank+chr*8+0);
	setVrom1KBank(1, 0x200*bank+chr*8+1);
	setVrom1KBank(2, 0x200*bank+chr*8+2);
	setVrom1KBank(3, 0x200*bank+chr*8+3);
	setVrom1KBank(4, 0x200*bank+chr*8+4);
	setVrom1KBank(5, 0x200*bank+chr*8+5);
	setVrom1KBank(6, 0x200*bank+chr*8+6);
	setVrom1KBank(7, 0x200*bank+chr*8+7);
}

#define STATE_SERIALIZE_BUILDER(sl) \
	STATE_SERIALIZE_BEGIN_##sl(Mapper255) \
	STATE_SERIALIZE_PARENT_##sl(NesMapper) \
	STATE_SERIALIZE_ARRAY_##sl(reg, sizeof(reg)) \
	STATE_SERIALIZE_END(Mapper255)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
