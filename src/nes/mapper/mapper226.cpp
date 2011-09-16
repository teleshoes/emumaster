#include "mapper226.h"
#include "ppu.h"
#include "disk.h"
#include <imachine.h>
#include <QDataStream>

void Mapper226::reset() {
	NesMapper::reset();

	setRom32KBank(0);

	reg[0] = 0;
	reg[1] = 0;
}

void Mapper226::writeHigh(quint16 address, quint8 data) {
	if (address & 0x001 )
		reg[1] = data;
	else
		reg[0] = data;

	if (reg[0] & 0x40)
		setMirroring(Vertical);
	else
		setMirroring(Horizontal);

	quint8 bank = ((reg[0]&0x1E)>>1)|((reg[0]&0x80)>>3)|((reg[1]&0x01)<<5);

	if (reg[0] & 0x20) {
		if (reg[0] & 0x01) {
			setRom8KBank(4, bank*4+2 );
			setRom8KBank(5, bank*4+3 );
			setRom8KBank(6, bank*4+2 );
			setRom8KBank(7, bank*4+3 );
		} else {
			setRom8KBank(4, bank*4+0 );
			setRom8KBank(5, bank*4+1 );
			setRom8KBank(6, bank*4+0 );
			setRom8KBank(7, bank*4+1 );
		}
	} else {
		setRom8KBank(4, bank*4+0 );
		setRom8KBank(5, bank*4+1 );
		setRom8KBank(6, bank*4+2 );
		setRom8KBank(7, bank*4+3 );
	}
}

#define STATE_SERIALIZE_BUILDER(sl) \
STATE_SERIALIZE_BEGIN_##sl(Mapper226, 1) \
	STATE_SERIALIZE_PARENT_##sl(NesMapper) \
	STATE_SERIALIZE_ARRAY_##sl(reg, sizeof(reg)) \
STATE_SERIALIZE_END_##sl(Mapper226)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
