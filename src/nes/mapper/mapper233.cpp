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

#include "mapper233.h"
#include "ppu.h"
#include "disk.h"

void Mapper233::reset() {
	NesMapper::reset();

	setRom32KBank(0);
}

void Mapper233::writeHigh(u16 address, u8 data) {
	Q_UNUSED(address)

	if (data & 0x20) {
		setRom8KBank(4, (data&0x1F)*2+0);
		setRom8KBank(5, (data&0x1F)*2+1);
		setRom8KBank(6, (data&0x1F)*2+0);
		setRom8KBank(7, (data&0x1F)*2+1);
	} else {
		u8 bank = (data&0x1E)>>1;

		setRom8KBank(4, bank*4+0);
		setRom8KBank(5, bank*4+1);
		setRom8KBank(6, bank*4+2);
		setRom8KBank(7, bank*4+3);
	}

	if ((data&0xC0) == 0x00)
		setMirroring(0, 0, 0, 1);
	else if ((data&0xC0) == 0x40)
		setMirroring(VerticalMirroring);
	else if ((data&0xC0) == 0x80)
		setMirroring(HorizontalMirroring);
	else
		setMirroring(SingleHigh);
}
