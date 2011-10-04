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

#include "mapper230.h"
#include "ppu.h"
#include "disk.h"
#include <imachine.h>
#include <QDataStream>

void Mapper230::reset() {
	NesMapper::reset();

	if (rom_sw)
		rom_sw = 0;
	else
		rom_sw = 1;
	if (rom_sw)
		setRom8KBanks(0, 1, 14, 15);
	else
		setRom8KBanks(16, 17, nesRomSize8KB-2, nesRomSize8KB-1);
}

void Mapper230::writeHigh(u16 address, u8 data) {
	Q_UNUSED(address)

	if (rom_sw) {
		setRom8KBank(4, (data&0x07)*2+0);
		setRom8KBank(5, (data&0x07)*2+1);
	} else {
		if (data & 0x20) {
			setRom8KBank(4, (data&0x1F)*2+16);
			setRom8KBank(5, (data&0x1F)*2+17);
			setRom8KBank(6, (data&0x1F)*2+16);
			setRom8KBank(7, (data&0x1F)*2+17);
		} else {
			setRom8KBank(4, (data&0x1E)*2+16);
			setRom8KBank(5, (data&0x1E)*2+17);
			setRom8KBank(6, (data&0x1E)*2+18);
			setRom8KBank(7, (data&0x1E)*2+19);
		}
		if (data & 0x40)
			setMirroring(VerticalMirroring);
		else
			setMirroring(HorizontalMirroring);
	}
}

#define STATE_SERIALIZE_BUILDER(sl) \
STATE_SERIALIZE_BEGIN_##sl(Mapper230, 1) \
	STATE_SERIALIZE_PARENT_##sl(NesMapper) \
	STATE_SERIALIZE_VAR_##sl(rom_sw) \
STATE_SERIALIZE_END_##sl(Mapper230)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
