#include "mapper243.h"
#include "ppu.h"
#include "disk.h"

void Mapper243::reset() {
	NesMapper::reset();

	setRom32KBank(0);

	if (nesVromSize8KB > 4)
		setVrom8KBank(4);
	else
		setVrom8KBank(0);

	setMirroring(VerticalMirroring);

	for (int i = 0; i < 4; i++)
		reg[i] = 0;
}

void Mapper243::writeLow(u16 address, u8 data) {
	if ((address&0x4101) == 0x4100) {
		reg[0] = data;
	} else if ((address&0x4101) == 0x4101) {
		switch (reg[0] & 0x07) {
		case 0:
			reg[1] = 0;
			reg[2] = 3;
			break;
		case 4:
			reg[2] = (reg[2]&0x06)|(data&0x01);
			break;
		case 5:
			reg[1] = data&0x01;
			break;
		case 6:
			reg[2] = (reg[2]&0x01)|((data&0x03)<<1);
			break;
		case 7:
			reg[3] = data&0x01;
			break;
		default:
			break;
		}

		setRom32KBank(reg[1]);
		setVrom8KBank(reg[2]);

		if (reg[3])
			setMirroring(VerticalMirroring);
		else
			setMirroring(HorizontalMirroring);
	}
}

#define STATE_SERIALIZE_BUILDER(sl) \
STATE_SERIALIZE_BEGIN_##sl(Mapper243, 1) \
	STATE_SERIALIZE_PARENT_##sl(NesMapper) \
	STATE_SERIALIZE_ARRAY_##sl(reg, sizeof(reg)) \
STATE_SERIALIZE_END_##sl(Mapper243)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
