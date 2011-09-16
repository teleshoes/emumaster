#include "mapper010.h"
#include "ppu.h"
#include "disk.h"
#include <QDataStream>

void Mapper010::reset() {
	NesMapper::reset();

	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);

	latch_a = 0xFE;
	latch_b = 0xFE;

	ppu()->setCharacterLatchEnabled(true);

	setVrom4KBank(0, 4);
	setVrom4KBank(4, 0);
}

void Mapper010::writeHigh(quint16 address, quint8 data) {
	switch (address & 0xF000) {
	case 0xA000:
		setRom8KBank(4, data);
		break;
	case 0xB000:
		reg[0] = data;
		if (latch_a == 0xFD) {
			setVrom4KBank(0, reg[0]);
		}
		break;
	case 0xC000:
		reg[1] = data;
		if (latch_a == 0xFE) {
			setVrom4KBank(0, reg[1]);
		}
		break;
	case 0xD000:
		reg[2] = data;
		if (latch_b == 0xFD) {
			setVrom4KBank(4, reg[2]);
		}
		break;
	case 0xE000:
		reg[3] = data;
		if (latch_b == 0xFE) {
			setVrom4KBank(4, reg[3]);
		}
		break;
	case 0xF000:
		if (data & 0x01)
			setMirroring(Horizontal);
		else
			setMirroring(Vertical);
		break;
	}
}

void Mapper010::characterLatch(quint16 address) {
	if ((address&0x1FF0) == 0x0FD0 && latch_a != 0xFD) {
		latch_a = 0xFD;
		setVrom4KBank(0, reg[0]);
	} else if ((address&0x1FF0) == 0x0FE0 && latch_a != 0xFE) {
		latch_a = 0xFE;
		setVrom4KBank(0, reg[1]);
	} else if ((address&0x1FF0) == 0x1FD0 && latch_b != 0xFD) {
		latch_b = 0xFD;
		setVrom4KBank(4, reg[2]);
	} else if ((address&0x1FF0) == 0x1FE0 && latch_b != 0xFE) {
		latch_b = 0xFE;
		setVrom4KBank(4, reg[3]);
	}
}

#define STATE_SERIALIZE_BUILDER(sl) \
STATE_SERIALIZE_BEGIN_##sl(Mapper010, 1) \
	STATE_SERIALIZE_PARENT_##sl(NesMapper) \
	STATE_SERIALIZE_ARRAY_##sl(reg, sizeof(reg)) \
	STATE_SERIALIZE_VAR_##sl(latch_a) \
	STATE_SERIALIZE_VAR_##sl(latch_b) \
STATE_SERIALIZE_END_##sl(Mapper010)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
