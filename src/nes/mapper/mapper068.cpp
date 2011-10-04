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

#include "mapper068.h"
#include "disk.h"
#include "ppu.h"
#include <QDataStream>

// TODO coin ???

void Mapper068::reset() {
	NesMapper::reset();

	reg[0] = reg[1] = reg[2] = reg[3] = 0;

	setRom8KBanks(0, 1, nesRomSize8KB-2, nesRomSize8KB-1);
}

void Mapper068::writeHigh(u16 address, u8 data) {
	switch (address & 0xF000) {
	case 0x8000:
		setVrom2KBank(0, data);
		break;
	case 0x9000:
		setVrom2KBank(2, data);
		break;
	case 0xA000:
		setVrom2KBank(4, data);
		break;
	case 0xB000:
		setVrom2KBank(6, data);
		break;

	case 0xC000:
		reg[2] = data;
		updateBanks();
		break;
	case 0xD000:
		reg[3] = data;
		updateBanks();
		break;
	case 0xE000:
		reg[0] = (data & 0x10)>>4;
		reg[1] = data & 0x03;
		updateBanks();
		break;

	case 0xF000:
		setRom16KBank(4, data);
		break;
	}
}

void Mapper068::updateBanks() {
	if (reg[0]) {
		switch (reg[1]) {
		case 0:
			setVrom1KBank( 8, reg[2]+0x80);
			setVrom1KBank( 9, reg[3]+0x80);
			setVrom1KBank(10, reg[2]+0x80);
			setVrom1KBank(11, reg[3]+0x80);
			break;
		case 1:
			setVrom1KBank( 8, reg[2]+0x80);
			setVrom1KBank( 9, reg[2]+0x80);
			setVrom1KBank(10, reg[3]+0x80);
			setVrom1KBank(11, reg[3]+0x80);
			break;
		case 2:
			setVrom1KBank( 8, reg[2]+0x80);
			setVrom1KBank( 9, reg[2]+0x80);
			setVrom1KBank(10, reg[2]+0x80);
			setVrom1KBank(11, reg[2]+0x80);
			break;
		case 3:
			setVrom1KBank( 8, reg[3]+0x80);
			setVrom1KBank( 9, reg[3]+0x80);
			setVrom1KBank(10, reg[3]+0x80);
			setVrom1KBank(11, reg[3]+0x80);
			break;
		}
	} else {
		setMirroring(static_cast<NesMirroring>(reg[1] & 0x03));
	}
}

#define STATE_SERIALIZE_BUILDER(sl) \
STATE_SERIALIZE_BEGIN_##sl(Mapper068, 1) \
	STATE_SERIALIZE_PARENT_##sl(NesMapper) \
	STATE_SERIALIZE_ARRAY_##sl(reg, sizeof(reg)) \
STATE_SERIALIZE_END_##sl(Mapper068)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
