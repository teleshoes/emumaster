#include "mapper012.h"
#include "ppu.h"
#include "disk.h"

void Mapper012::reset() {
	NesMapper::reset();

	qMemSet(reg, 0, sizeof(reg));

	prg0 = 0;
	prg1 = 1;
	updateCpuBanks();

	irq_enable = 0;	// Disable
	irq_counter = 0;
	irq_latch = 0xFF;
	irq_request = 0;
	irq_preset = 0;
	irq_preset_vbl = 0;

	vb0 = 0;
	vb1 = 0;
	chr01 = 0;
	chr23 = 2;
	chr4  = 4;
	chr5  = 5;
	chr6  = 6;
	chr7  = 7;
	updatePpuBanks();
}

u8 Mapper012::readLow(u16 address) {
	Q_UNUSED(address)
	return 0x01;
}

void Mapper012::writeLow(u16 address, u8 data) {
	if (address > 0x4100 && address < 0x6000) {
		vb0 = (data&0x01)<<8;
		vb1 = (data&0x10)<<4;
		updatePpuBanks();
	} else {
		Mapper012::writeLow(address, data);
	}
}

void Mapper012::writeHigh(u16 address, u8 data) {
	switch (address & 0xE001) {
	case 0x8000:
		reg[0] = data;
		updateCpuBanks();
		updatePpuBanks();
		break;
	case 0x8001:
		reg[1] = data;
		switch (reg[0] & 0x07) {
		case 0x00: chr01 = data & 0xFE; updatePpuBanks(); break;
		case 0x01: chr23 = data & 0xFE; updatePpuBanks(); break;
		case 0x02: chr4 = data; updatePpuBanks(); break;
		case 0x03: chr5 = data; updatePpuBanks(); break;
		case 0x04: chr6 = data; updatePpuBanks(); break;
		case 0x05: chr7 = data; updatePpuBanks(); break;
		case 0x06: prg0 = data; updateCpuBanks(); break;
		case 0x07: prg1 = data; updateCpuBanks(); break;
		}
		break;
	case 0xA000:
		reg[2] = data;
		if (nesMirroring != FourScreenMirroring) {
			if (data & 0x01)
				setMirroring(HorizontalMirroring);
			else
				setMirroring(VerticalMirroring);
		}
		break;
	case 0xA001:
		reg[3] = data;
		break;
	case 0xC000:
		reg[4] = data;
		irq_latch = data;
		break;
	case 0xC001:
		reg[5] = data;
		if (nesPpuScanline < NesPpu::VisibleScreenHeight) {
			irq_counter |= 0x80;
			irq_preset = 0xFF;
		} else {
			irq_counter |= 0x80;
			irq_preset_vbl = 0xFF;
			irq_preset = 0;
		}
		break;
	case 0xE000:
		reg[6] = data;
		irq_enable = 0;
		irq_request = 0;
		setIrqSignalOut(false);
		break;
	case 0xE001:
		reg[7] = data;
		irq_enable = 1;
		irq_request = 0;
		break;
	}
}

void Mapper012::updateCpuBanks() {
	if (reg[0] & 0x40)
		setRom8KBanks(nesRomSize8KB-2, prg1, prg0, nesRomSize8KB-1);
	else
		setRom8KBanks(prg0, prg1, nesRomSize8KB-2, nesRomSize8KB-1);
}

void Mapper012::horizontalSync(int scanline) {
	if (scanline < NesPpu::VisibleScreenHeight && nesPpu.isDisplayOn()) {
		if (irq_preset_vbl) {
			irq_counter = irq_latch;
			irq_preset_vbl = 0;
		}
		if (irq_preset) {
			irq_counter = irq_latch;
			irq_preset = 0;
		} else if (irq_counter > 0) {
			irq_counter--;
		}
		if (irq_counter == 0) {
			if (irq_enable) {
				irq_request = 0xFF;
				setIrqSignalOut(true);
			}
			irq_preset = 0xFF;
		}
	}
}

void Mapper012::updatePpuBanks() {
	if (nesVromSize1KB) {
		if (reg[0] & 0x80) {
			setVrom1KBank(4, vb1 + chr01);
			setVrom1KBank(5, vb1 + chr01+1);
			setVrom1KBank(6, vb1 + chr23);
			setVrom1KBank(7, vb1 + chr23+1);
			setVrom1KBank(0, vb0 + chr4);
			setVrom1KBank(1, vb0 + chr5);
			setVrom1KBank(2, vb0 + chr6);
			setVrom1KBank(3, vb0 + chr7);
		} else {
			setVrom1KBank(0, vb0 + chr01);
			setVrom1KBank(1, vb0 + chr01+1);
			setVrom1KBank(2, vb0 + chr23);
			setVrom1KBank(3, vb0 + chr23+1);
			setVrom1KBank(4, vb1 + chr4);
			setVrom1KBank(5, vb1 + chr5);
			setVrom1KBank(6, vb1 + chr6);
			setVrom1KBank(7, vb1 + chr7);
		}
	} else {
		if (reg[0] & 0x80) {
			setCram1KBank(4, (chr01+0)&0x07);
			setCram1KBank(5, (chr01+1)&0x07);
			setCram1KBank(6, (chr23+0)&0x07);
			setCram1KBank(7, (chr23+1)&0x07);
			setCram1KBank(0, chr4&0x07);
			setCram1KBank(1, chr5&0x07);
			setCram1KBank(2, chr6&0x07);
			setCram1KBank(3, chr7&0x07);
		} else {
			setCram1KBank(0, (chr01+0)&0x07);
			setCram1KBank(1, (chr01+1)&0x07);
			setCram1KBank(2, (chr23+0)&0x07);
			setCram1KBank(3, (chr23+1)&0x07);
			setCram1KBank(4, chr4&0x07);
			setCram1KBank(5, chr5&0x07);
			setCram1KBank(6, chr6&0x07);
			setCram1KBank(7, chr7&0x07);
		}
	}
}

#define STATE_SERIALIZE_BUILDER(sl) \
STATE_SERIALIZE_BEGIN_##sl(Mapper012, 1) \
	STATE_SERIALIZE_PARENT_##sl(NesMapper) \
	STATE_SERIALIZE_ARRAY_##sl(reg, sizeof(reg)) \
	STATE_SERIALIZE_VAR_##sl(prg0) \
	STATE_SERIALIZE_VAR_##sl(prg1) \
	STATE_SERIALIZE_VAR_##sl(chr01) \
	STATE_SERIALIZE_VAR_##sl(chr23) \
	STATE_SERIALIZE_VAR_##sl(chr4) \
	STATE_SERIALIZE_VAR_##sl(chr5) \
	STATE_SERIALIZE_VAR_##sl(chr6) \
	STATE_SERIALIZE_VAR_##sl(chr7) \
	STATE_SERIALIZE_VAR_##sl(irq_enable) \
	STATE_SERIALIZE_VAR_##sl(irq_counter) \
	STATE_SERIALIZE_VAR_##sl(irq_latch) \
	STATE_SERIALIZE_VAR_##sl(irq_request) \
	STATE_SERIALIZE_VAR_##sl(irq_preset) \
	STATE_SERIALIZE_VAR_##sl(irq_preset_vbl) \
	STATE_SERIALIZE_VAR_##sl(vb0) \
	STATE_SERIALIZE_VAR_##sl(vb1) \
STATE_SERIALIZE_END_##sl(Mapper012)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
