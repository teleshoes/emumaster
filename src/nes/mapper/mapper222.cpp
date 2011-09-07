#include "mapper222.h"
#include "nesppu.h"
#include "nesdisk.h"

void Mapper222::reset() {
	NesMapper::reset();

	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);
	if (vromSize1KB())
		setVrom8KBank(0);
	setMirroring(Vertical);
}

void Mapper222::writeHigh(quint16 address, quint8 data) {
	switch (address & 0xF003) {
	case 0x8000:
		setRom8KBank(4, data);
		break;
	case 0xA000:
		setRom8KBank(5, data);
		break;
	case 0xB000:
		setVrom1KBank(0, data);
		break;
	case 0xB002:
		setVrom1KBank(1, data);
		break;
	case 0xC000:
		setVrom1KBank(2, data);
		break;
	case 0xC002:
		setVrom1KBank(3, data);
		break;
	case 0xD000:
		setVrom1KBank(4, data);
		break;
	case 0xD002:
		setVrom1KBank(5, data);
		break;
	case 0xE000:
		setVrom1KBank(6, data);
		break;
	case 0xE002:
		setVrom1KBank(7, data);
		break;
	}
}
