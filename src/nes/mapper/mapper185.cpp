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

#include "mapper185.h"
#include "disk.h"

void Mapper185::reset()
{
	if (nesRomSize16KB == 1) {
		setRom16KBank(4, 0);
		setRom16KBank(6, 0);
	} else {
		setRom32KBank(0);
	}

	for (int i = 0; i < 0x400; i++)
		nesVram[0x800+i] = 0xFF;

	patch = 0;

	uint crc = nesDiskCrc;
	if (crc == 0xb36457c7) {	// Spy vs Spy(J)
		patch = 1;
	}
}

void Mapper185::writeHigh(u16 addr, u8 data)
{
	if ((!patch && (data&0x03)) || (patch && data == 0x21)) {
		setVrom8KBank(0);
	} else {
		setVram1KBank(0, 2);	// use vram bank 2
		setVram1KBank(1, 2);
		setVram1KBank(2, 2);
		setVram1KBank(3, 2);
		setVram1KBank(4, 2);
		setVram1KBank(5, 2);
		setVram1KBank(6, 2);
		setVram1KBank(7, 2);
	}
}
