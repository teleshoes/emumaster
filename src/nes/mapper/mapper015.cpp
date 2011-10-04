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

#include "mapper015.h"

void Mapper015::reset() {
	NesMapper::reset();
	setRom32KBank(0);
}

void Mapper015::writeHigh(u16 address, u8 data) {
	switch (address) {
	case 0x8000:
		if (data & 0x80) {
			setRom8KBank(4, (data&0x3F)*2+1);
			setRom8KBank(5, (data&0x3F)*2+0);
			setRom8KBank(6, (data&0x3F)*2+3);
			setRom8KBank(7, (data&0x3F)*2+2);
		} else {
			setRom8KBank(4, (data&0x3F)*2+0);
			setRom8KBank(5, (data&0x3F)*2+1);
			setRom8KBank(6, (data&0x3F)*2+2);
			setRom8KBank(7, (data&0x3F)*2+3);
		}
		if (data & 0x40)
			setMirroring(HorizontalMirroring);
		else
			setMirroring(VerticalMirroring);
		break;
	case 0x8001:
		if (data & 0x80) {
			setRom8KBank(6, (data&0x3F)*2+1);
			setRom8KBank(7, (data&0x3F)*2+0);
		} else {
			setRom8KBank(6, (data&0x3F)*2+0);
			setRom8KBank(7, (data&0x3F)*2+1);
		}
		break;
	case 0x8002:
		if (data & 0x80) {
			setRom8KBank(4, (data&0x3F)*2+1);
			setRom8KBank(5, (data&0x3F)*2+1);
			setRom8KBank(6, (data&0x3F)*2+1);
			setRom8KBank(7, (data&0x3F)*2+1);
		} else {
			setRom8KBank(4, (data&0x3F)*2+0);
			setRom8KBank(5, (data&0x3F)*2+0);
			setRom8KBank(6, (data&0x3F)*2+0);
			setRom8KBank(7, (data&0x3F)*2+0);
		}
		break;
	case 0x8003:
		if (data & 0x80) {
			setRom8KBank(6, (data&0x3F)*2+1);
			setRom8KBank(7, (data&0x3F)*2+0);
		} else {
			setRom8KBank(6, (data&0x3F)*2+0);
			setRom8KBank(7, (data&0x3F)*2+1);
		}
		if (data & 0x40)
			setMirroring(HorizontalMirroring);
		else
			setMirroring(VerticalMirroring);
		break;
	}
}
