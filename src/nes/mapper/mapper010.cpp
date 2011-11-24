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

#include "mapper010.h"
#include "ppu.h"
#include "disk.h"
#include <QDataStream>

void Mapper010::reset() {
	NesMapper::reset();

	setRom8KBanks(0, 1, nesRomSize8KB-2, nesRomSize8KB-1);

	latch_a = 0xFE;
	latch_b = 0xFE;

	nesPpu.setCharacterLatchEnabled(true);

	setVrom4KBank(0, 4);
	setVrom4KBank(4, 0);
}

void Mapper010::writeHigh(u16 address, u8 data) {
	switch (address & 0xF000) {
	case 0xA000:
		setRom8KBank(4, data);
		break;
	case 0xB000:
		reg[0] = data;
		if (latch_a == 0xFD) {
			setVrom4KBank(0, reg[0]);
		}
		break;
	case 0xC000:
		reg[1] = data;
		if (latch_a == 0xFE) {
			setVrom4KBank(0, reg[1]);
		}
		break;
	case 0xD000:
		reg[2] = data;
		if (latch_b == 0xFD) {
			setVrom4KBank(4, reg[2]);
		}
		break;
	case 0xE000:
		reg[3] = data;
		if (latch_b == 0xFE) {
			setVrom4KBank(4, reg[3]);
		}
		break;
	case 0xF000:
		if (data & 0x01)
			setMirroring(HorizontalMirroring);
		else
			setMirroring(VerticalMirroring);
		break;
	}
}

void Mapper010::characterLatch(u16 address) {
	if ((address&0x1FF0) == 0x0FD0 && latch_a != 0xFD) {
		latch_a = 0xFD;
		setVrom4KBank(0, reg[0]);
	} else if ((address&0x1FF0) == 0x0FE0 && latch_a != 0xFE) {
		latch_a = 0xFE;
		setVrom4KBank(0, reg[1]);
	} else if ((address&0x1FF0) == 0x1FD0 && latch_b != 0xFD) {
		latch_b = 0xFD;
		setVrom4KBank(4, reg[2]);
	} else if ((address&0x1FF0) == 0x1FE0 && latch_b != 0xFE) {
		latch_b = 0xFE;
		setVrom4KBank(4, reg[3]);
	}
}

void Mapper010::extSl() {
	emsl.array("reg", reg, sizeof(reg));
	emsl.var("latch_a", latch_a);
	emsl.var("latch_b", latch_b);
}
