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

#include "mapper046.h"
#include <QDataStream>

void Mapper046::reset() {
	NesMapper::reset();
	qMemSet(reg, 0, 4);
	updateBanks();
	setMirroring(VerticalMirroring);
}

void Mapper046::writeLow(u16 address, u8 data) {
	Q_UNUSED(address)
	reg[0] = data & 0x0F;
	reg[1] = (data & 0xF0) >> 4;
	updateBanks();
}

void Mapper046::writeHigh(u16 address, u8 data) {
	Q_UNUSED(address)
	reg[2] = data & 0x01;
	reg[3] = (data & 0x70) >> 4;
	updateBanks();
}

void Mapper046::updateBanks() {
	setRom32KBank(reg[0]*2+reg[2]);
	setVrom8KBank(reg[1]*8+reg[3]);
}

#define STATE_SERIALIZE_BUILDER(sl) \
STATE_SERIALIZE_BEGIN_##sl(Mapper046, 1) \
	STATE_SERIALIZE_PARENT_##sl(NesMapper) \
	STATE_SERIALIZE_ARRAY_##sl(reg, sizeof(reg)) \
STATE_SERIALIZE_END_##sl(Mapper046)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
