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

#include "mapper007.h"
#include "ppu.h"
#include "disk.h"

void Mapper007::reset() {
	NesMapper::reset();
	patch = 0;
	setRom32KBank(0);
	setMirroring(SingleLow);

	u32 crc = nesDiskCrc;
	if( crc == 0x3c9fe649 ) {	// WWF Wrestlemania Challenge(U)
		setMirroring(VerticalMirroring);
		patch = 1;
	}
	if( crc == 0x09874777 ) {	// Marble Madness(U)
		nesEmu.setRenderMethod(NesEmu::TileRender);
	}

	if( crc == 0x279710DC		// Battletoads (U)
	 || crc == 0xCEB65B06 ) {	// Battletoads Double Dragon (U)
		nesEmu.setRenderMethod(NesEmu::PreAllRender);
		qMemSet(nesWram, 0, sizeof(nesWram));
	}
}

void Mapper007::writeHigh(u16 address, u8 data) {
	Q_UNUSED(address)
	setRom32KBank(data & 0x07);
	if (!patch) {
		if (data & 0x10)
			setMirroring(SingleHigh);
		else
			setMirroring(SingleLow);
	}
}
