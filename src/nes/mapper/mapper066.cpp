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

#include "mapper066.h"

void Mapper066::reset() {
	NesMapper::reset();

	setRom8KBanks(0, 1, nesRomSize8KB-2, nesRomSize8KB-1);
	setVrom8KBank(0);
}

void Mapper066::writeLow(u16 address, u8 data) {
	if(address >= 0x6000) {
		setRom32KBank((data & 0xF0) >> 4);
		setVrom8KBank(data & 0x0F);
	}
}

void Mapper066::writeHigh(u16 address, u8 data) {
	Q_UNUSED(address)
	setRom32KBank((data & 0xF0) >> 4);
	setVrom8KBank(data & 0x0F);
}
