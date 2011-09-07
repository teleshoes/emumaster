#include "mapper227.h"
#include "nesppu.h"
#include "nesdisk.h"

void Mapper227::reset() {
	NesMapper::reset();

	setRom8KBanks(0, 1, 0, 1);
}

void Mapper227::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(data)

	quint8 bank = ((address&0x0100)>>4) | ((address&0x0078)>>3);

	if (address & 0x0001) {
		setRom32KBank(bank);
	} else {
		if (address & 0x0004) {
			setRom8KBank(4, bank*4+2);
			setRom8KBank(5, bank*4+3);
			setRom8KBank(6, bank*4+2);
			setRom8KBank(7, bank*4+3);
		} else {
			setRom8KBank(4, bank*4+0);
			setRom8KBank(5, bank*4+1);
			setRom8KBank(6, bank*4+0);
			setRom8KBank(7, bank*4+1);
		}
	}

	if (!(address & 0x0080)) {
		if (address & 0x0200) {
			setRom8KBank(6, (bank&0x1C)*4+14);
			setRom8KBank(7, (bank&0x1C)*4+15);
		} else {
			setRom8KBank(6, (bank&0x1C)*4+0);
			setRom8KBank(7, (bank&0x1C)*4+1);
		}
	}
	if (address & 0x0002)
		setMirroring(Horizontal);
	else
		setMirroring(Vertical);
}
