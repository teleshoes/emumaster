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

#include "mapper060.h"
#include "disk.h"
#include <QDataStream>

void Mapper060::reset() {
	NesMapper::reset();

	patch = 0;
	u32 crc = nesDiskCrc;
	if (crc == 0xf9c484a0) {	// Reset Based 4-in-1(Unl)
		setRom16KBank(4, game_sel);
		setRom16KBank(6, game_sel);
		setVrom8KBank(game_sel);
		game_sel++;
		game_sel &= 3;
	} else {
		patch = 1;
		setRom32KBank(0);
		setVrom8KBank(0);
	}
}

void Mapper060::writeHigh(u16 address, u8 data) {
	if (patch) {
		if (address & 0x80) {
			setRom16KBank(4, (address & 0x70) >> 4);
			setRom16KBank(6, (address & 0x70) >> 4);
		} else {
			setRom32KBank((address & 0x70) >> 5);
		}
		setVrom8KBank(address & 0x07);
		setMirroring(static_cast<NesMirroring>((data & 0x08) >> 3));
	}
}

void Mapper060::extSl() {
	emsl.var("game_sel", game_sel);
}
