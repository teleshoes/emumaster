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

#include "mapper011.h"

void Mapper011::reset() {
	NesMapper::reset();
	setRom32KBank(0);
	if (nesVromSize1KB)
		setVrom8KBank(0);
	setMirroring(VerticalMirroring);
}

void Mapper011::writeHigh(u16 address, u8 data) {
	Q_UNUSED(address)
	setRom32KBank(data);
	if (nesVromSize1KB)
		setVrom8KBank(data >> 4);
}
