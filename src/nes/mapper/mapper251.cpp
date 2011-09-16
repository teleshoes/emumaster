#include "mapper251.h"
#include "ppu.h"
#include "disk.h"
#include <imachine.h>
#include <QDataStream>

void Mapper251::reset() {
	NesMapper::reset();

	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);
	setMirroring(Vertical);

	qMemSet(reg, 0, sizeof(reg));
	qMemSet(breg, 0, sizeof(breg));
}

void Mapper251::writeLow(quint16 address, quint8 data) {
	Q_UNUSED(address)
	if ((address & 0xE001) == 0x6000) {
		if (reg[9]) {
			breg[reg[10]++] = data;
			if (reg[10] == 4) {
				reg[10] = 0;
				setBank();
			}
		}
	}
}

void Mapper251::writeHigh(quint16 address, quint8 data) {
	switch (address & 0xE001) {
	case 0x8000:
		reg[8] = data;
		setBank();
		break;
	case 0x8001:
		reg[reg[8]&0x07] = data;
		setBank();
		break;
	case 0xA001:
		if (data & 0x80) {
			reg[ 9] = 1;
			reg[10] = 0;
		} else {
			reg[ 9] = 0;
		}
		break;
	}
}

void Mapper251::setBank() {
	int chr[6];
	int prg[4];

	for (int i = 0; i < 6; i++)
		chr[i] = (reg[i]|(breg[1]<<4)) & ((breg[2]<<4)|0x0F);

	if (reg[8] & 0x80) {
		setVrom1KBank(4, chr[0]);
		setVrom1KBank(5, chr[0]+1);
		setVrom1KBank(6, chr[1]);
		setVrom1KBank(7, chr[1]+1);
		setVrom1KBank(0, chr[2]);
		setVrom1KBank(1, chr[3]);
		setVrom1KBank(2, chr[4]);
		setVrom1KBank(3, chr[5]);
	} else {
		setVrom1KBank(0, chr[0]);
		setVrom1KBank(1, chr[0]+1);
		setVrom1KBank(2, chr[1]);
		setVrom1KBank(3, chr[1]+1);
		setVrom1KBank(4, chr[2]);
		setVrom1KBank(5, chr[3]);
		setVrom1KBank(6, chr[4]);
		setVrom1KBank(7, chr[5]);
	}

	prg[0] = (reg[6]&((breg[3]&0x3F)^0x3F))|(breg[1]);
	prg[1] = (reg[7]&((breg[3]&0x3F)^0x3F))|(breg[1]);
	prg[2] = prg[3] =((breg[3]&0x3F)^0x3F)|(breg[1]);
	prg[2] &= romSize8KB()-1;

	if (reg[8] & 0x40)
		setRom8KBanks(prg[2],prg[1],prg[0],prg[3]);
	else
		setRom8KBanks(prg[0],prg[1],prg[2],prg[3]);
}

#define STATE_SERIALIZE_BUILDER(sl) \
STATE_SERIALIZE_BEGIN_##sl(Mapper251, 1) \
	STATE_SERIALIZE_PARENT_##sl(NesMapper) \
	STATE_SERIALIZE_ARRAY_##sl(reg, sizeof(reg)) \
	STATE_SERIALIZE_ARRAY_##sl(breg, sizeof(breg)) \
STATE_SERIALIZE_END_##sl(Mapper251)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
