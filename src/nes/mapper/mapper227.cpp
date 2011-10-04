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

#include "mapper227.h"
#include "ppu.h"
#include "disk.h"

void Mapper227::reset() {
	NesMapper::reset();

	setRom8KBanks(0, 1, 0, 1);
}

void Mapper227::writeHigh(u16 address, u8 data) {
	Q_UNUSED(data)

	u8 bank = ((address&0x0100)>>4) | ((address&0x0078)>>3);

	if (address & 0x0001) {
		setRom32KBank(bank);
	} else {
		if (address & 0x0004) {
			setRom8KBank(4, bank*4+2);
			setRom8KBank(5, bank*4+3);
			setRom8KBank(6, bank*4+2);
			setRom8KBank(7, bank*4+3);
		} else {
			setRom8KBank(4, bank*4+0);
			setRom8KBank(5, bank*4+1);
			setRom8KBank(6, bank*4+0);
			setRom8KBank(7, bank*4+1);
		}
	}

	if (!(address & 0x0080)) {
		if (address & 0x0200) {
			setRom8KBank(6, (bank&0x1C)*4+14);
			setRom8KBank(7, (bank&0x1C)*4+15);
		} else {
			setRom8KBank(6, (bank&0x1C)*4+0);
			setRom8KBank(7, (bank&0x1C)*4+1);
		}
	}
	if (address & 0x0002)
		setMirroring(HorizontalMirroring);
	else
		setMirroring(VerticalMirroring);
}
