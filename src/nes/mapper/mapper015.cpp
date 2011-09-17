#include "mapper015.h"

void Mapper015::reset() {
	NesMapper::reset();
	setRom32KBank(0);
}

void Mapper015::writeHigh(u16 address, u8 data) {
	switch (address) {
	case 0x8000:
		if (data & 0x80) {
			setRom8KBank(4, (data&0x3F)*2+1);
			setRom8KBank(5, (data&0x3F)*2+0);
			setRom8KBank(6, (data&0x3F)*2+3);
			setRom8KBank(7, (data&0x3F)*2+2);
		} else {
			setRom8KBank(4, (data&0x3F)*2+0);
			setRom8KBank(5, (data&0x3F)*2+1);
			setRom8KBank(6, (data&0x3F)*2+2);
			setRom8KBank(7, (data&0x3F)*2+3);
		}
		if (data & 0x40)
			setMirroring(HorizontalMirroring);
		else
			setMirroring(VerticalMirroring);
		break;
	case 0x8001:
		if (data & 0x80) {
			setRom8KBank(6, (data&0x3F)*2+1);
			setRom8KBank(7, (data&0x3F)*2+0);
		} else {
			setRom8KBank(6, (data&0x3F)*2+0);
			setRom8KBank(7, (data&0x3F)*2+1);
		}
		break;
	case 0x8002:
		if (data & 0x80) {
			setRom8KBank(4, (data&0x3F)*2+1);
			setRom8KBank(5, (data&0x3F)*2+1);
			setRom8KBank(6, (data&0x3F)*2+1);
			setRom8KBank(7, (data&0x3F)*2+1);
		} else {
			setRom8KBank(4, (data&0x3F)*2+0);
			setRom8KBank(5, (data&0x3F)*2+0);
			setRom8KBank(6, (data&0x3F)*2+0);
			setRom8KBank(7, (data&0x3F)*2+0);
		}
		break;
	case 0x8003:
		if (data & 0x80) {
			setRom8KBank(6, (data&0x3F)*2+1);
			setRom8KBank(7, (data&0x3F)*2+0);
		} else {
			setRom8KBank(6, (data&0x3F)*2+0);
			setRom8KBank(7, (data&0x3F)*2+1);
		}
		if (data & 0x40)
			setMirroring(HorizontalMirroring);
		else
			setMirroring(VerticalMirroring);
		break;
	}
}
