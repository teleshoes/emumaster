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

#include "mapper062.h"

void Mapper062::reset() {
	NesMapper::reset();

	setRom32KBank(0);
	setVrom8KBank(0);
}

void Mapper062::writeHigh(u16 address, u8 data) {
	switch (address & 0xFF00) {
	case 0x8100:
		setRom8KBank(4, data);
		setRom8KBank(5, data+1);
		break;
	case 0x8500:
		setRom8KBank(4, data);
		break;
	case 0x8700:
		setRom8KBank(5, data);
		break;
	}
	for (int i = 0; i < 8; i++)
		setVrom1KBank(i, data+i);
}
