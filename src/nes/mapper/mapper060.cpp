#include "mapper060.h"
#include "disk.h"
#include <QDataStream>

void Mapper060::reset() {
	NesMapper::reset();

	patch = 0;
	quint32 crc = disk()->crc();
	if (crc == 0xf9c484a0) {	// Reset Based 4-in-1(Unl)
		setRom16KBank(4, game_sel);
		setRom16KBank(6, game_sel);
		setVrom8KBank(game_sel);
		game_sel++;
		game_sel &= 3;
	} else {
		patch = 1;
		setRom32KBank(0);
		setVrom8KBank(0);
	}
}

void Mapper060::writeHigh(quint16 address, quint8 data) {
	if (patch) {
		if (address & 0x80) {
			setRom16KBank(4, (address & 0x70) >> 4);
			setRom16KBank(6, (address & 0x70) >> 4);
		} else {
			setRom32KBank((address & 0x70) >> 5);
		}
		setVrom8KBank(address & 0x07);
		setMirroring(static_cast<Mirroring>((data & 0x08) >> 3));
	}
}

#define STATE_SERIALIZE_BUILDER(sl) \
STATE_SERIALIZE_BEGIN_##sl(Mapper060, 1) \
	STATE_SERIALIZE_PARENT_##sl(NesMapper) \
	STATE_SERIALIZE_VAR_##sl(game_sel) \
STATE_SERIALIZE_END_##sl(Mapper060)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
