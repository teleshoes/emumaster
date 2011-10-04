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

#include "mapper202.h"
#include "ppu.h"
#include "disk.h"

void Mapper202::reset() {
	NesMapper::reset();

	setRom16KBank(4, 6);
	setRom16KBank(6, 7);
	if (nesVromSize1KB)
		setVrom8KBank(0);
}

void Mapper202::writeEx(u16 address, u8 data) {
	if (address >= 0x4020)
		writeSub(address, data);
}

void Mapper202::writeLow(u16 address, u8 data) {
	writeSub(address, data);
}

void Mapper202::writeHigh(u16 address, u8 data) {
	writeSub(address, data);
}

void Mapper202::writeSub(u16 address, u8 data) {
	Q_UNUSED(data)

	int bank = (address>>1) & 0x07;
	setRom16KBank(4, bank);
	if ((address & 0x0C) == 0x0C)
		setRom16KBank(6, bank+1);
	else
		setRom16KBank(6, bank);
	setVrom8KBank(bank);

	if (address & 0x01)
		setMirroring(HorizontalMirroring);
	else
		setMirroring(VerticalMirroring);
}
