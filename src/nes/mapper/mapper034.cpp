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

#include "mapper034.h"

void Mapper034::reset() {
	NesMapper::reset();

	setRom8KBanks(0, 1, nesRomSize8KB-2, nesRomSize8KB-1);
	if (nesVromSize1KB)
		setVrom8KBank(0);
}

void Mapper034::writeLow(u16 address, u8 data) {
	if (address == 0x7FFD)
		setRom32KBank(data);
	else if (address == 0x7FFE)
		setVrom4KBank(0, data);
	else if (address == 0x7FFF)
		setVrom4KBank(4, data);
}

void Mapper034::writeHigh(u16 address, u8 data) {
	Q_UNUSED(address)
	setRom32KBank(data);
}
