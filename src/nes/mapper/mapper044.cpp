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

#include "mapper044.h"
#include "disk.h"
#include "ppu.h"
#include <QDataStream>

void Mapper044::reset() {
	NesMapper::reset();

	patch = 0;

	u32 crc = nesDiskCrc;
	if (crc == 0x7eef434c) {
		patch = 1;
	}

	memset(reg, 0, sizeof(reg));

	bank = 0;
	prg0 = 0;
	prg1 = 1;

	// set VROM banks
	if (nesVromSize1KB) {
		chr01 = 0;
		chr23 = 2;
		chr4  = 4;
		chr5  = 5;
		chr6  = 6;
		chr7  = 7;
	} else {
		chr01 = chr23 = chr4 = chr5 = chr6 = chr7 = 0;
	}

	setBankCpu();
	setBankPpu();

	irq_enable = 0;
	irq_counter = 0;
	irq_latch = 0;
}

void Mapper044::writeLow(u16 address, u8 data) {
	if (address == 0x6000) {
		if (patch) {
			bank = (data & 0x06) >> 1;
		} else {
			bank = (data & 0x01) << 1;
		}
		setBankCpu();
		setBankPpu();
	}
}

void Mapper044::writeHigh(u16 address, u8 data) {
	switch (address & 0xE001) {
	case 0x8000:
		reg[0] = data;
		setBankCpu();
		setBankPpu();
		break;
	case 0x8001:
		reg[1] = data;
		switch( reg[0] & 0x07) {
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
		bank = data & 0x07;
		if (bank == 7) {
			bank = 6;
		}
		setBankCpu();
		setBankPpu();
		break;
	case 0xC000:
		reg[4] = data;
		irq_counter = data;
		break;
	case 0xC001:
		reg[5] = data;
		irq_latch = data;
		break;
	case 0xE000:
		reg[6] = data;
		irq_enable = 0;
		setIrqSignalOut(false);
		break;
	case 0xE001:
		reg[7] = data;
		irq_enable = 1;
		break;
	}
}

void Mapper044::setBankCpu() {
	if (reg[0] & 0x40) {
		setRom8KBank(4, ((bank == 6)?0x1e:0x0e)|(bank<<4));
		setRom8KBank(5, ((bank == 6)?0x1f&prg1:0x0f&prg1)|(bank<<4));
		setRom8KBank(6, ((bank == 6)?0x1f&prg0:0x0f&prg0)|(bank<<4));
		setRom8KBank(7, ((bank == 6)?0x1f:0x0f)|(bank<<4));
	} else {
		setRom8KBank(4, ((bank == 6)?0x1f&prg0:0x0f&prg0)|(bank<<4));
		setRom8KBank(5, ((bank == 6)?0x1f&prg1:0x0f&prg1)|(bank<<4));
		setRom8KBank(6, ((bank == 6)?0x1e:0x0e)|(bank<<4));
		setRom8KBank(7, ((bank == 6)?0x1f:0x0f)|(bank<<4));
	}
}

void Mapper044::setBankPpu() {
	if (nesVromSize1KB) {
		if (reg[0] & 0x80) {
			setVrom1KBank(0, ((bank == 6)?0xff&chr4:0x7f&chr4)|(bank<<7));
			setVrom1KBank(1, ((bank == 6)?0xff&chr5:0x7f&chr5)|(bank<<7));
			setVrom1KBank(2, ((bank == 6)?0xff&chr6:0x7f&chr6)|(bank<<7));
			setVrom1KBank(3, ((bank == 6)?0xff&chr7:0x7f&chr7)|(bank<<7));
			setVrom1KBank(4, ((bank == 6)?0xff&chr01:0x7f&chr01)|(bank<<7));
			setVrom1KBank(5, ((bank == 6)?0xff&(chr01+1):0x7f&(chr01+1))|(bank<<7));
			setVrom1KBank(6, ((bank == 6)?0xff&chr23:0x7f&chr23)|(bank<<7));
			setVrom1KBank(7, ((bank == 6)?0xff&(chr23+1):0x7f&(chr23+1))|(bank<<7));
		} else {
			setVrom1KBank(0, ((bank == 6)?0xff&chr01:0x7f&chr01)|(bank<<7));
			setVrom1KBank(1, ((bank == 6)?0xff&(chr01+1):0x7f&(chr01+1))|(bank<<7));
			setVrom1KBank(2, ((bank == 6)?0xff&chr23:0x7f&chr23)|(bank<<7));
			setVrom1KBank(3, ((bank == 6)?0xff&(chr23+1):0x7f&(chr23+1))|(bank<<7));
			setVrom1KBank(4, ((bank == 6)?0xff&chr4:0x7f&chr4)|(bank<<7));
			setVrom1KBank(5, ((bank == 6)?0xff&chr5:0x7f&chr5)|(bank<<7));
			setVrom1KBank(6, ((bank == 6)?0xff&chr6:0x7f&chr6)|(bank<<7));
			setVrom1KBank(7, ((bank == 6)?0xff&chr7:0x7f&chr7)|(bank<<7));
		}
	}
}

void Mapper044::horizontalSync() {
	if (nesPpuScanline < NesPpu::VisibleScreenHeight && nesPpu.isDisplayOn()) {
		if (irq_enable) {
			if (!(--irq_counter)) {
				irq_counter = irq_latch;
				setIrqSignalOut(true);
			}
		}
	}
}

void Mapper044::extSl() {
	emsl.array("reg", reg, sizeof(reg));
	emsl.var("bank", bank);
	emsl.var("prg0", prg0);
	emsl.var("prg1", prg1);
	emsl.var("chr01", chr01);
	emsl.var("chr23", chr23);
	emsl.var("chr4", chr4);
	emsl.var("chr5", chr5);
	emsl.var("chr6", chr6);
	emsl.var("chr7", chr7);
	emsl.var("irq_enable", irq_enable);
	emsl.var("irq_counter", irq_counter);
	emsl.var("irq_latch", irq_latch);
}
