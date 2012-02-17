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

#include "mapper255.h"
#include "ppu.h"
#include "disk.h"
#include <emu.h>
#include <QDataStream>

void Mapper255::reset() {
	NesMapper::reset();

	setRom32KBank(0);
	setVrom8KBank(0);
	setMirroring(VerticalMirroring);

	for (int i = 0; i < 4; i++)
		reg[i] = 0;
}

u8 Mapper255::readLow(u16 address) {
	if (address >= 0x5800)
		return reg[address & 0x0003] & 0x0F;
	else
		return address >> 8;
}

void Mapper255::writeLow(u16 address, u8 data) {
	if (address >= 0x5800)
		reg[address & 0x0003] = data & 0x0F;
}

void Mapper255::writeHigh(u16 address, u8 data) {
	Q_UNUSED(data)

	u8 prg = (address & 0x0F80) >> 7;
	int chr = (address & 0x003F);
	int bank = (address & 0x4000) >> 14;

	if (address & 0x2000)
		setMirroring(HorizontalMirroring);
	else
		setMirroring(VerticalMirroring);

	if (address & 0x1000) {
		if (address & 0x0040) {
			setRom8KBank(4, 0x80*bank+prg*4+2);
			setRom8KBank(5, 0x80*bank+prg*4+3);
			setRom8KBank(6, 0x80*bank+prg*4+2);
			setRom8KBank(7, 0x80*bank+prg*4+3);
		} else {
			setRom8KBank(4, 0x80*bank+prg*4+0);
			setRom8KBank(5, 0x80*bank+prg*4+1);
			setRom8KBank(6, 0x80*bank+prg*4+0);
			setRom8KBank(7, 0x80*bank+prg*4+1);
		}
	} else {
		setRom8KBank(4, 0x80*bank+prg*4+0);
		setRom8KBank(5, 0x80*bank+prg*4+1);
		setRom8KBank(6, 0x80*bank+prg*4+2);
		setRom8KBank(7, 0x80*bank+prg*4+3);
	}

	setVrom1KBank(0, 0x200*bank+chr*8+0);
	setVrom1KBank(1, 0x200*bank+chr*8+1);
	setVrom1KBank(2, 0x200*bank+chr*8+2);
	setVrom1KBank(3, 0x200*bank+chr*8+3);
	setVrom1KBank(4, 0x200*bank+chr*8+4);
	setVrom1KBank(5, 0x200*bank+chr*8+5);
	setVrom1KBank(6, 0x200*bank+chr*8+6);
	setVrom1KBank(7, 0x200*bank+chr*8+7);
}

void Mapper255::extSl() {
	emsl.array("reg", reg, sizeof(reg));
}
