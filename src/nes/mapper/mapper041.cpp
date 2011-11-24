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

#include "mapper041.h"
#include <QDataStream>

void Mapper041::reset() {
	NesMapper::reset();

	reg[0] = reg[1] = 0;
	setRom32KBank(0);
	if (nesVromSize1KB)
		setVrom8KBank(0);
}

void Mapper041::writeLow(u16 address, u8 data) {
	Q_UNUSED(data)
	if (address >= 0x6000 && address < 0x6800) {
		setRom32KBank(address & 0x07);
		reg[0] = address & 0x04;
		reg[1] &= 0x03;
		reg[1] |= (address>>1) & 0x0C;
		setVrom8KBank(reg[1]);
		setMirroring(static_cast<NesMirroring>((data & 0x20) >> 5));
	}
}

void Mapper041::writeHigh(u16 address, u8 data) {
	Q_UNUSED(data)
	if (reg[0]) {
		reg[1] &= 0x0C;
		reg[1] &= address & 0x03;
		setVrom8KBank(reg[1]);
	}
}

void Mapper041::extSl() {
	emsl.array("reg", reg, sizeof(reg));
}
