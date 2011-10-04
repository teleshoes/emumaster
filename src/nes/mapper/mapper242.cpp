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

#include "mapper242.h"
#include "ppu.h"
#include "disk.h"

void Mapper242::reset() {
	NesMapper::reset();
	setRom32KBank(0);
}

void Mapper242::writeHigh(u16 address, u8 data) {
	Q_UNUSED(data)
	if (address & 0x01)
		setRom32KBank((address & 0xF8) >> 3);
}
