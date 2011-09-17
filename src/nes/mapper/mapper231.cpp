#include "mapper231.h"
#include "ppu.h"
#include "disk.h"

void Mapper231::reset() {
	NesMapper::reset();

	setRom32KBank(0);
	if (nesVromSize1KB)
		setVrom8KBank(0);
}

void Mapper231::writeHigh(u16 address, u8 data) {
	Q_UNUSED(data)

	if (address & 0x0020) {
		setRom32KBank(u8(address>>1));
	} else {
		u8 bank = address & 0x1E;
		setRom8KBank(4, bank*2+0);
		setRom8KBank(5, bank*2+1);
		setRom8KBank(6, bank*2+0);
		setRom8KBank(7, bank*2+1);
	}

	if (address & 0x0080)
		setMirroring(HorizontalMirroring);
	else
		setMirroring(VerticalMirroring);
}
