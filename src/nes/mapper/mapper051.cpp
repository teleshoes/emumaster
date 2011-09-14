#include "mapper051.h"
#include <QDataStream>

void Mapper051::reset() {
	NesMapper::reset();

	bank = 0;
	mode = 1;
	updateBanks();
	setCram8KBank(0);
}

void Mapper051::writeLow(quint16 address, quint8 data) {
	if (address >= 0x6000) {
		mode = ((data & 0x10) >> 3) | ((data & 0x02) >> 1);
		updateBanks();
	}
}

void Mapper051::writeHigh(quint16 address, quint8 data) {
	bank = (data & 0x0f) << 2;
	if (0xC000 <= address && address <= 0xDFFF)
		mode = (mode & 0x01) | ((data & 0x10) >> 3);
	updateBanks();
}

void Mapper051::updateBanks() {
	switch (mode) {
	case 0:
		setMirroring(Vertical);
		setRom8KBank(3, (bank|0x2c|3));
		setRom8KBank(4, (bank|0x00|0));
		setRom8KBank(5, (bank|0x00|1));
		setRom8KBank(6, (bank|0x0c|2));
		setRom8KBank(7, (bank|0x0c|3));
		break;
	case 1:
		setMirroring(Vertical);
		setRom8KBank(3, (bank|0x20|3));
		setRom8KBank(4, (bank|0x00|0));
		setRom8KBank(5, (bank|0x00|1));
		setRom8KBank(6, (bank|0x00|2));
		setRom8KBank(7, (bank|0x00|3));
		break;
	case 2:
		setMirroring(Vertical);
		setRom8KBank(3, (bank|0x2e|3));
		setRom8KBank(4, (bank|0x02|0));
		setRom8KBank(5, (bank|0x02|1));
		setRom8KBank(6, (bank|0x0e|2));
		setRom8KBank(7, (bank|0x0e|3));
		break;
	case 3:
		setMirroring(Horizontal);
		setRom8KBank(3, (bank|0x20|3));
		setRom8KBank(4, (bank|0x00|0));
		setRom8KBank(5, (bank|0x00|1));
		setRom8KBank(6, (bank|0x00|2));
		setRom8KBank(7, (bank|0x00|3));
		break;
	}
}

#define STATE_SERIALIZE_BUILDER(sl) \
	STATE_SERIALIZE_BEGIN_##sl(Mapper051) \
	STATE_SERIALIZE_PARENT_##sl(NesMapper) \
	STATE_SERIALIZE_VAR_##sl(mode) \
	STATE_SERIALIZE_VAR_##sl(bank) \
	STATE_SERIALIZE_END(Mapper051)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)