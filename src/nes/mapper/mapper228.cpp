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

#include "mapper228.h"
#include "ppu.h"
#include "disk.h"

void Mapper228::reset() {
	NesMapper::reset();

	setRom32KBank(0);
	setVrom8KBank(0);
}

void Mapper228::writeHigh(u16 address, u8 data) {
	u8 prg = (address&0x0780) >> 7;

	switch ((address&0x1800) >> 11) {
	case 1:
		prg |= 0x10;
		break;
	case 3:
		prg |= 0x20;
		break;
	}

	if (address & 0x0020) {
		prg <<= 1;
		if (address & 0x0040)
			prg++;
		setRom8KBank(4, prg*4+0);
		setRom8KBank(5, prg*4+1);
		setRom8KBank(6, prg*4+0);
		setRom8KBank(7, prg*4+1);
	} else {
		setRom8KBank(4, prg*4+0);
		setRom8KBank(5, prg*4+1);
		setRom8KBank(6, prg*4+2);
		setRom8KBank(7, prg*4+3);
	}

	setVrom8KBank(((address&0x000F)<<2)|(data&0x03));

	if (address & 0x2000)
		setMirroring(HorizontalMirroring);
	else
		setMirroring(VerticalMirroring);
}
