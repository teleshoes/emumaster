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

#include "mapper022.h"

void Mapper022::reset() {
	NesMapper::reset();
	setRom8KBanks(0, 1, nesRomSize8KB-2, nesRomSize8KB-1);
}

void Mapper022::writeHigh(u16 address, u8 data) {
	switch (address) {
	case 0x8000:
		setRom8KBank(4, data);
		break;
	case 0x9000:
		setMirroring(static_cast<NesMirroring>(data & 3));
		break;
	case 0xA000: setRom8KBank(5, data); break;
	case 0xB000: setVrom1KBank(0, data >> 1); break;
	case 0xB001: setVrom1KBank(1, data >> 1); break;
	case 0xC000: setVrom1KBank(2, data >> 1); break;
	case 0xC001: setVrom1KBank(3, data >> 1); break;
	case 0xD000: setVrom1KBank(4, data >> 1); break;
	case 0xD001: setVrom1KBank(5, data >> 1); break;
	case 0xE000: setVrom1KBank(6, data >> 1); break;
	case 0xE001: setVrom1KBank(7, data >> 1); break;
	}
}
