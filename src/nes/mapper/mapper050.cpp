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

#include "mapper050.h"
#include "ppu.h"
#include <QDataStream>

void Mapper050::reset() {
	NesMapper::reset();

	irq_enable = 0;
	setRom8KBank(3, 15);
	setRom8KBank(4, 8);
	setRom8KBank(5, 9);
	setRom8KBank(6, 0);
	setRom8KBank(7, 11);
	if (nesVromSize1KB)
		setVrom8KBank(0);
}

void Mapper050::writeEx(u16 address, u8 data) {
	if ((address & 0xE060) == 0x4020) {
		if (address & 0x0100) {
			irq_enable = data & 0x01;
			setIrqSignalOut(false);
		} else {
			setRom8KBank(6, (data&0x08)|((data&0x01)<<2)|((data&0x06)>>1));
		}
	}
}

void Mapper050::writeLow(u16 address, u8 data) {
	if ((address & 0xE060) == 0x4020) {
		if (address & 0x0100) {
			irq_enable = data & 0x01;
			setIrqSignalOut(false);
		} else {
			setRom8KBank(6, (data&0x08)|((data&0x01)<<2)|((data&0x06)>>1));
		}
	}
}

void Mapper050::horizontalSync() {
	if (irq_enable) {
		if (nesPpuScanline == 21)
			setIrqSignalOut(true);
	}
}

void Mapper050::extSl() {
	emsl.var("irq_enable", irq_enable);
}
