#include "mapper225.h"
#include "ppu.h"
#include "disk.h"

void Mapper225::reset() {
	NesMapper::reset();

	setRom32KBank(0);
	if (vromSize1KB())
		setVrom8KBank(0);
}

void Mapper225::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(data)

	quint8 prg_bank = (address & 0x0F80) >> 7;
	quint8 chr_bank = address & 0x003F;

	setVrom1KBank(0, chr_bank*8+0);
	setVrom1KBank(1, chr_bank*8+1);
	setVrom1KBank(2, chr_bank*8+2);
	setVrom1KBank(3, chr_bank*8+3);
	setVrom1KBank(4, chr_bank*8+4);
	setVrom1KBank(5, chr_bank*8+5);
	setVrom1KBank(6, chr_bank*8+6);
	setVrom1KBank(7, chr_bank*8+7);

	if (address & 0x2000)
		setMirroring(Horizontal);
	else
		setMirroring(Vertical);

	if (address & 0x1000) {
		if (address & 0x0040) {
			setRom8KBank(4, prg_bank*4+2);
			setRom8KBank(5, prg_bank*4+3);
			setRom8KBank(6, prg_bank*4+2);
			setRom8KBank(7, prg_bank*4+3);
		} else {
			setRom8KBank(4, prg_bank*4+0);
			setRom8KBank(5, prg_bank*4+1);
			setRom8KBank(6, prg_bank*4+0);
			setRom8KBank(7, prg_bank*4+1);
		}
	} else {
		setRom8KBank(4, prg_bank*4+0);
		setRom8KBank(5, prg_bank*4+1);
		setRom8KBank(6, prg_bank*4+2);
		setRom8KBank(7, prg_bank*4+3);
	}
}
