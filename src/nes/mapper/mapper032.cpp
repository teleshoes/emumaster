#include "mapper032.h"
#include "nesdisk.h"
#include <QDataStream>

void Mapper032::reset() {
	NesMapper::reset();

	patch = 0;
	reg = 0;

	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);
	if (vromSize1KB())
		setVrom8KBank(0);

	quint32 crc = disk()->crc();
	// For Major League(J)
	if (crc == 0xc0fed437) {
		patch = 1;
	}
	// For Ai Sensei no Oshiete - Watashi no Hoshi(J)
	if (crc == 0xfd3fc292) {
		setRom8KBanks(30, 31, 30, 31);
	}
}

void Mapper032::writeHigh(quint16 address, quint8 data) {
	switch (address & 0xF000) {
	case 0x8000:
		if (reg & 0x02)
			setRom8KBank(6, data);
		else
			setRom8KBank(4, data);
		break;
	case 0x9000:
		reg = data;
		setMirroring(static_cast<Mirroring>(data & 0x01));
		break;
	case 0xA000:
		setRom8KBank(1, data);
		break;
	}
	switch (address & 0xF007) {
	case 0xB000:
	case 0xB001:
	case 0xB002:
	case 0xB003:
	case 0xB004:
	case 0xB005:
		setVrom1KBank(address & 0x0007, data);
		break;
	case 0xB006:
		setVrom1KBank(6, data);
		if (patch && (data & 0x40))
			setMirroring(0, 0, 0, 1);
		break;
	case 0xB007:
		setVrom1KBank(7, data);
		if (patch && (data & 0x40))
			setMirroring(SingleLow);
		break;
	}
}

#define STATE_SERIALIZE_BUILDER(sl) \
	STATE_SERIALIZE_BEGIN_##sl(Mapper032) \
	STATE_SERIALIZE_PARENT_##sl(NesMapper) \
	STATE_SERIALIZE_VAR_##sl(reg) \
	STATE_SERIALIZE_END(Mapper032)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)