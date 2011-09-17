#include "mapper233.h"
#include "ppu.h"
#include "disk.h"

void Mapper233::reset() {
	NesMapper::reset();

	setRom32KBank(0);
}

void Mapper233::writeHigh(u16 address, u8 data) {
	Q_UNUSED(address)

	if (data & 0x20) {
		setRom8KBank(4, (data&0x1F)*2+0);
		setRom8KBank(5, (data&0x1F)*2+1);
		setRom8KBank(6, (data&0x1F)*2+0);
		setRom8KBank(7, (data&0x1F)*2+1);
	} else {
		u8 bank = (data&0x1E)>>1;

		setRom8KBank(4, bank*4+0);
		setRom8KBank(5, bank*4+1);
		setRom8KBank(6, bank*4+2);
		setRom8KBank(7, bank*4+3);
	}

	if ((data&0xC0) == 0x00)
		setMirroring(0, 0, 0, 1);
	else if ((data&0xC0) == 0x40)
		setMirroring(VerticalMirroring);
	else if ((data&0xC0) == 0x80)
		setMirroring(HorizontalMirroring);
	else
		setMirroring(SingleHigh);
}
