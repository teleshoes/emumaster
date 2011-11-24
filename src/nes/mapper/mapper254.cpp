/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "mapper254.h"
#include "ppu.h"
#include "disk.h"
#include <imachine.h>
#include <QDataStream>

void Mapper254::reset() {
	NesMapper::reset();

	for (int i = 0; i < 8; i++)
		reg[i] = 0x00;

	protectflag = 0;

	prg0 = 0;
	prg1 = 1;
	setBankCpu();

	chr01 = 0;
	chr23 = 2;
	chr4  = 4;
	chr5  = 5;
	chr6  = 6;
	chr7  = 7;
	setBankPpu();

	irq_enable = 0;	// Disable
	irq_counter = 0;
	irq_latch = 0;
	irq_request = 0;
}

u8 Mapper254::readLow(u16 address) {
	if (address >= 0x6000) {
		if (protectflag)
			return readDirect(address);
		else
			return readDirect(address) ^ 0x01;
	}
	return NesMapper::readLow(address);
}

void Mapper254::writeLow(u16 address, u8 data) {
	switch (address & 0xF000) {
	case 0x6000:
	case 0x7000:
		writeDirect(address, data);
		break;
	}
}

void Mapper254::writeHigh(u16 address, u8 data) {
	switch (address & 0xE001) {
	case 0x8000:
		protectflag = 0xFF;
		reg[0] = data;
		setBankCpu();
		setBankPpu();
		break;
	case 0x8001:
		reg[1] = data;

		switch (reg[0] & 0x07) {
		case 0x00: chr01 = data & 0xFE; setBankPpu(); break;
		case 0x01: chr23 = data & 0xFE; setBankPpu(); break;
		case 0x02: chr4 = data; setBankPpu(); break;
		case 0x03: chr5 = data; setBankPpu(); break;
		case 0x04: chr6 = data; setBankPpu(); break;
		case 0x05: chr7 = data; setBankPpu(); break;
		case 0x06: prg0 = data; setBankCpu(); break;
		case 0x07: prg1 = data; setBankCpu(); break;
		}
		break;
	case 0xA000:
		reg[2] = data;
		if (nesMirroring != FourScreenMirroring)
			setMirroring(static_cast<NesMirroring>(data & 0x01));
		break;
	case 0xA001:
		reg[3] = data;
		break;
	case 0xC000:
		reg[4] = data;
		irq_counter = data;
		irq_request = 0;
		setIrqSignalOut(false);
		break;
	case 0xC001:
		reg[5] = data;
		irq_latch = data;
		irq_request = 0;
		setIrqSignalOut(false);
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
		setIrqSignalOut(false);
		break;
	}
}

void Mapper254::horizontalSync() {
	if (nesPpuScanline < NesPpu::VisibleScreenHeight && nesPpu.isDisplayOn()) {
		if (irq_enable && !irq_request) {
			if (nesPpuScanline == 0) {
				if (irq_counter)
					irq_counter--;
			}
			if (!(irq_counter--)) {
				irq_request = 0xFF;
				irq_counter = irq_latch;
				setIrqSignalOut(true);
			}
		}
	}
}

void Mapper254::setBankCpu() {
	if (reg[0] & 0x40)
		setRom8KBanks(nesRomSize8KB-2, prg1, prg0, nesRomSize8KB-1);
	else
		setRom8KBanks(prg0, prg1, nesRomSize8KB-2, nesRomSize8KB-1);
}

void Mapper254::setBankPpu() {
	if (nesVromSize1KB) {
		if (reg[0] & 0x80) {
			setVrom1KBank(4, (chr01+0));
			setVrom1KBank(5, (chr01+1));
			setVrom1KBank(6, (chr23+0));
			setVrom1KBank(7, (chr23+1));
			setVrom1KBank(0, chr4);
			setVrom1KBank(1, chr5);
			setVrom1KBank(2, chr6);
			setVrom1KBank(3, chr7);
		} else {
			setVrom1KBank(0, (chr01+0));
			setVrom1KBank(1, (chr01+1));
			setVrom1KBank(2, (chr23+0));
			setVrom1KBank(3, (chr23+1));
			setVrom1KBank(4, chr4);
			setVrom1KBank(5, chr5);
			setVrom1KBank(6, chr6);
			setVrom1KBank(7, chr7);
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

void Mapper254::extSl() {
	emsl.array("reg", reg, sizeof(reg));
	emsl.var("prg0", prg0);
	emsl.var("prg1", prg1);
	emsl.var("chr01", chr01);
	emsl.var("chr23", chr23);
	emsl.var("chr4", chr4);
	emsl.var("chr5", chr5);
	emsl.var("chr6", chr6);
	emsl.var("chr7", chr7);
	emsl.var("irq_type", irq_type);
	emsl.var("irq_enable", irq_enable);
	emsl.var("irq_counter", irq_counter);
	emsl.var("irq_latch", irq_latch);
	emsl.var("irq_request", irq_request);
	emsl.var("protectflag", protectflag);
}
