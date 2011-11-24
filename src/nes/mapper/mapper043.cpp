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

#include "mapper043.h"
#include <QDataStream>

void Mapper043::reset() {
	NesMapper::reset();

	setRom8KBank(3, 2);
	setRom8KBanks(1, 0, 4, 9);
	if (nesVromSize1KB)
		setVrom8KBank(0);
	irq_enable = 0xFF;
	irq_counter = 0;
}

u8 Mapper043::readLow(u16 address) {
	if (0x5000 <= address && address < 0x6000)
		return	nesRom[0x2000*8+0x1000+(address-0x5000)];
	return address >> 8;
}

void Mapper043::writeEx(u16 address, u8 data) {
	if ((address&0xF0FF) == 0x4022) {
		switch (data&0x07) {
		case 0x00:
		case 0x02:
		case 0x03:
		case 0x04:
			setRom8KBank(6, 4);
			break;
		case 0x01:
			setRom8KBank(6, 3);
			break;
		case 0x05:
			setRom8KBank(6, 7);
			break;
		case 0x06:
			setRom8KBank(6, 5);
			break;
		case 0x07:
			setRom8KBank(6, 6);
			break;
		}
	}
}

void Mapper043::writeLow(u16 address, u8 data) {
	if ((address&0xF0FF) == 0x4022)
		writeEx(address, data);
}

void Mapper043::writeHigh(u16 address, u8 data) {
	if (address == 0x8122) {
		if (data & 0x03) {
			irq_enable = 1;
		} else {
			irq_counter = 0;
			irq_enable = 0;
		}
		setIrqSignalOut(false);
	}
}

void Mapper043::horizontalSync() {
	setIrqSignalOut(false);
	if (irq_enable) {
		irq_counter += 341;
		if (irq_counter >= 12288) {
			irq_counter = 0;
			setIrqSignalOut(true);
		}
	}
}

void Mapper043::extSl() {
	emsl.var("irq_enable", irq_enable);
	emsl.var("irq_counter", irq_counter);
}
