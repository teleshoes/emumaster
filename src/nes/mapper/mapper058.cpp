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

#include "mapper058.h"

void Mapper058::reset() {
	NesMapper::reset();

	setRom8KBanks(0, 1, 0, 1);
	if (nesVromSize1KB)
		setVrom8KBank(0);
}

void Mapper058::writeHigh(u16 address, u8 data) {
	if (address & 0x40) {
		setRom16KBank(4, address & 0x07);
		setRom16KBank(6, address & 0x07);
	} else {
		setRom32KBank((address & 0x06) >> 1);
	}
	if (nesVromSize1KB)
		setVrom8KBank((address & 0x38) >> 3);
	setMirroring(static_cast<NesMirroring>((data & 0x02) >> 1));
}
