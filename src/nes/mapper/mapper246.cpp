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

#include "mapper246.h"
#include "ppu.h"
#include "disk.h"

void Mapper246::reset() {
	NesMapper::reset();

	setRom8KBanks(0, 1, nesRomSize8KB-2, nesRomSize8KB-1);
}

void Mapper246::writeHigh(u16 address, u8 data) {
	if (address >= 0x6000 && address < 0x8000) {
		switch( address) {
		case 0x6000:
			setRom8KBank(4, data);
			break;
		case 0x6001:
			setRom8KBank(5, data);
			break;
		case 0x6002:
			setRom8KBank(6, data);
			break;
		case 0x6003:
			setRom8KBank(7, data);
			break;
		case 0x6004:
			setVrom2KBank(0, data);
			break;
		case 0x6005:
			setVrom2KBank(2, data);
			break;
		case 0x6006:
			setVrom2KBank(4, data);
			break;
		case 0x6007:
			setVrom2KBank(6, data);
			break;
		default:
			writeDirect(address, data);
			break;
		}
	}
}
