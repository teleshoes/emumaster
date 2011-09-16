#include "mapper236.h"
#include "ppu.h"
#include "disk.h"

void Mapper236::reset() {
	NesMapper::reset();

	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);
	bank = mode = 0;
}

void Mapper236::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(data)

	if (address >= 0x8000 && address <= 0xBFFF) {
		bank = ((address&0x03)<<4)|(bank&0x07);
	} else {
		bank = (address&0x07)|(bank&0x30);
		mode = address & 0x30;
	}

	if (address & 0x20)
		setMirroring(Horizontal);
	else
		setMirroring(Vertical);

	switch (mode) {
	case 0x00:
		bank |= 0x08;
		setRom8KBank(4, bank*2+0);
		setRom8KBank(5, bank*2+1);
		setRom8KBank(6, (bank|0x07)*2+0);
		setRom8KBank(7, (bank|0x07)*2+1);
		break;
	case 0x10:
		bank |= 0x37;
		setRom8KBank(4, bank*2+0);
		setRom8KBank(5, bank*2+1);
		setRom8KBank(6, (bank|0x07)*2+0);
		setRom8KBank(7, (bank|0x07)*2+1);
		break;
	case 0x20:
		bank |= 0x08;
		setRom8KBank(4, (bank&0xFE)*2+0);
		setRom8KBank(5, (bank&0xFE)*2+1);
		setRom8KBank(6, (bank&0xFE)*2+2);
		setRom8KBank(7, (bank&0xFE)*2+3);
		break;
	case 0x30:
		bank |= 0x08;
		setRom8KBank(4, bank*2+0);
		setRom8KBank(5, bank*2+1);
		setRom8KBank(6, bank*2+0);
		setRom8KBank(7, bank*2+1);
		break;
	}
}

#define STATE_SERIALIZE_BUILDER(sl) \
STATE_SERIALIZE_BEGIN_##sl(Mapper236, 1) \
	STATE_SERIALIZE_PARENT_##sl(NesMapper) \
	STATE_SERIALIZE_VAR_##sl(bank) \
	STATE_SERIALIZE_VAR_##sl(mode) \
STATE_SERIALIZE_END_##sl(Mapper236)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
