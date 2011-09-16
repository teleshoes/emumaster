#include "mapper229.h"
#include "ppu.h"
#include "disk.h"

void Mapper229::reset() {
	NesMapper::reset();

	setRom32KBank(0);
	setVrom8KBank(0);
}

void Mapper229::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(data)

	if (address & 0x001E) {
		quint8 prg = address & 0x001F;

		setRom8KBank(4, prg*2+0);
		setRom8KBank(5, prg*2+1);
		setRom8KBank(6, prg*2+0);
		setRom8KBank(7, prg*2+1);

		setVrom8KBank(address & 0x0FFF);
	} else {
		setRom32KBank(0);
		setVrom8KBank(0);
	}
	if (address & 0x0020)
		setMirroring(Horizontal);
	else
		setMirroring(Vertical);
}
