#include "mapper235.h"
#include "ppu.h"
#include "disk.h"

void Mapper235::reset() {
	NesMapper::reset();

	qMemSet(m_wram+0x2000, 0x2000, 0xFF);
	setRom32KBank(0);
}

void Mapper235::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(data)

	quint8 prg = ((address&0x0300)>>3) | (address&0x001F);
	quint8 bus = 0;

	if (romSize8KB() == 64*2) {
		switch (address & 0x0300) {
		case 0x0000: break;
		case 0x0100: bus = 1; break;
		case 0x0200: bus = 1; break;
		case 0x0300: bus = 1; break;
		}
	} else if (romSize8KB() == 128*2) {
		switch (address & 0x0300) {
		case 0x0000: break;
		case 0x0100: bus = 1; break;
		case 0x0200: prg = (prg&0x1F)|0x20; break;
		case 0x0300: bus = 1; break;
		}
	} else if (romSize8KB() == 192*2) {
		switch (address & 0x0300) {
		case 0x0000: break;
		case 0x0100: bus = 1; break;
		case 0x0200: prg = (prg&0x1F)|0x20; break;
		case 0x0300: prg = (prg&0x1F)|0x40; break;
		}
	}

	if (address & 0x0800) {
		if (address & 0x1000) {
			setRom8KBank(4, prg*4+2);
			setRom8KBank(5, prg*4+3);
			setRom8KBank(6, prg*4+2);
			setRom8KBank(7, prg*4+3);
		} else {
			setRom8KBank(4, prg*4+0);
			setRom8KBank(5, prg*4+1);
			setRom8KBank(6, prg*4+0);
			setRom8KBank(7, prg*4+1);
		}
	} else {
		setRom32KBank(prg);
	}

	if (bus) {
		for (int i = 4; i < 8; i++)
			setWram8KBank(i, 1);
	}

	if (address & 0x0400)
		setMirroring(SingleLow);
	else if (address & 0x2000)
		setMirroring(Horizontal);
	else
		setMirroring(Vertical);
}
