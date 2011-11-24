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

#include "mapper236.h"
#include "ppu.h"
#include "disk.h"

void Mapper236::reset() {
	NesMapper::reset();

	setRom8KBanks(0, 1, nesRomSize8KB-2, nesRomSize8KB-1);
	bank = mode = 0;
}

void Mapper236::writeHigh(u16 address, u8 data) {
	Q_UNUSED(data)

	if (address >= 0x8000 && address <= 0xBFFF) {
		bank = ((address&0x03)<<4)|(bank&0x07);
	} else {
		bank = (address&0x07)|(bank&0x30);
		mode = address & 0x30;
	}

	if (address & 0x20)
		setMirroring(HorizontalMirroring);
	else
		setMirroring(VerticalMirroring);

	switch (mode) {
	case 0x00:
		bank |= 0x08;
		setRom8KBank(4, bank*2+0);
		setRom8KBank(5, bank*2+1);
		setRom8KBank(6, (bank|0x07)*2+0);
		setRom8KBank(7, (bank|0x07)*2+1);
		break;
	case 0x10:
		bank |= 0x37;
		setRom8KBank(4, bank*2+0);
		setRom8KBank(5, bank*2+1);
		setRom8KBank(6, (bank|0x07)*2+0);
		setRom8KBank(7, (bank|0x07)*2+1);
		break;
	case 0x20:
		bank |= 0x08;
		setRom8KBank(4, (bank&0xFE)*2+0);
		setRom8KBank(5, (bank&0xFE)*2+1);
		setRom8KBank(6, (bank&0xFE)*2+2);
		setRom8KBank(7, (bank&0xFE)*2+3);
		break;
	case 0x30:
		bank |= 0x08;
		setRom8KBank(4, bank*2+0);
		setRom8KBank(5, bank*2+1);
		setRom8KBank(6, bank*2+0);
		setRom8KBank(7, bank*2+1);
		break;
	}
}

void Mapper236::extSl() {
	emsl.var("bank", bank);
	emsl.var("mode", mode);
}
