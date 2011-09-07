#include "mapper022.h"

void Mapper022::reset() {
	NesMapper::reset();
	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);
}

void Mapper022::writeHigh(quint16 address, quint8 data) {
	switch (address) {
	case 0x8000:
		setRom8KBank(4, data);
		break;
	case 0x9000:
		setMirroring(static_cast<Mirroring>(data & 3));
		break;
	case 0xA000: setRom8KBank(5, data); break;
	case 0xB000: setVrom1KBank(0, data >> 1); break;
	case 0xB001: setVrom1KBank(1, data >> 1); break;
	case 0xC000: setVrom1KBank(2, data >> 1); break;
	case 0xC001: setVrom1KBank(3, data >> 1); break;
	case 0xD000: setVrom1KBank(4, data >> 1); break;
	case 0xD001: setVrom1KBank(5, data >> 1); break;
	case 0xE000: setVrom1KBank(6, data >> 1); break;
	case 0xE001: setVrom1KBank(7, data >> 1); break;
	}
}
