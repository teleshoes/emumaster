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

#include "mapper070.h"
#include "disk.h"
#include "ppu.h"

void Mapper070::reset() {
	NesMapper::reset();

	patch = 0;

	u32 crc = nesDiskCrc;
	if (crc == 0xa59ca2ef) {	// Kamen Rider Club(J)
		patch = 1;
		nesEmuSetRenderMethod(NesEmu::PostAllRender);
	}
	if (crc == 0x10bb8f9a) {	// Family Trainer - Manhattan Police(J)
		patch = 1;
	}
	if (crc == 0x0cd00488) {	// Space Shadow(J)
		patch = 1;
	}
	setRom8KBanks(0, 1, nesRomSize8KB-2, nesRomSize8KB-1);
	setVrom8KBank(0);
}

void Mapper070::writeHigh(u16 address, u8 data) {
	Q_UNUSED(address)
	setRom16KBank(4, (data >> 4) & 7);
	setVrom8KBank(data & 0x0F);
	if (patch) {
		if (data & 0x80)
			setMirroring(HorizontalMirroring);
		else
			setMirroring(VerticalMirroring);
	} else {
		if (data & 0x80)
			setMirroring(SingleHigh);
		else
			setMirroring(SingleLow);
	}
}
