#include "mapper230.h"
#include "nesppu.h"
#include "nesdisk.h"
#include <imachine.h>
#include <QDataStream>

void Mapper230::reset() {
	NesMapper::reset();

	if (rom_sw)
		rom_sw = 0;
	else
		rom_sw = 1;
	if (rom_sw)
		setRom8KBanks(0, 1, 14, 15);
	else
		setRom8KBanks(16, 17, romSize8KB()-2, romSize8KB()-1);
}

void Mapper230::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(address)

	if (rom_sw) {
		setRom8KBank(4, (data&0x07)*2+0);
		setRom8KBank(5, (data&0x07)*2+1);
	} else {
		if (data & 0x20) {
			setRom8KBank(4, (data&0x1F)*2+16);
			setRom8KBank(5, (data&0x1F)*2+17);
			setRom8KBank(6, (data&0x1F)*2+16);
			setRom8KBank(7, (data&0x1F)*2+17);
		} else {
			setRom8KBank(4, (data&0x1E)*2+16);
			setRom8KBank(5, (data&0x1E)*2+17);
			setRom8KBank(6, (data&0x1E)*2+18);
			setRom8KBank(7, (data&0x1E)*2+19);
		}
		if (data & 0x40)
			setMirroring(Vertical);
		else
			setMirroring(Horizontal);
	}
}

#define STATE_SERIALIZE_BUILDER(sl) \
	STATE_SERIALIZE_BEGIN_##sl(Mapper230) \
	STATE_SERIALIZE_PARENT_##sl(NesMapper) \
	STATE_SERIALIZE_VAR_##sl(rom_sw) \
	STATE_SERIALIZE_END(Mapper230)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
