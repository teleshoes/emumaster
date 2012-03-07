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

#include "mapper062.h"

void Mapper062::reset()
{
	NesMapper::reset();

	setRom32KBank(0);
	setVrom8KBank(0);
}

void Mapper062::writeHigh(u16 addr, u8 data)
{
	setVrom8KBank(((addr&0x1F)<<2)|(data&0x03));

	if(addr&0x20) {
		setRom16KBank(0x8000>>13,(addr&0x40)|((addr>>8)&0x3F));
		setRom16KBank(0xc000>>13,(addr&0x40)|((addr>>8)&0x3F));
	}
	else
		setRom32KBank(((addr&0x40)|((addr>>8)&0x3F))>>1);
	setMirroring(static_cast<NesMirroring>(((addr&0x80)>>7)^1));
}
