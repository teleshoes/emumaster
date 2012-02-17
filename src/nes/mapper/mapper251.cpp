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

#include "mapper251.h"
#include "ppu.h"
#include "disk.h"
#include <emu.h>
#include <QDataStream>

void Mapper251::reset() {
	NesMapper::reset();

	setRom8KBanks(0, 1, nesRomSize8KB-2, nesRomSize8KB-1);
	setMirroring(VerticalMirroring);

	qMemSet(reg, 0, sizeof(reg));
	qMemSet(breg, 0, sizeof(breg));
}

void Mapper251::writeLow(u16 address, u8 data) {
	Q_UNUSED(address)
	if ((address & 0xE001) == 0x6000) {
		if (reg[9]) {
			breg[reg[10]++] = data;
			if (reg[10] == 4) {
				reg[10] = 0;
				setBank();
			}
		}
	}
}

void Mapper251::writeHigh(u16 address, u8 data) {
	switch (address & 0xE001) {
	case 0x8000:
		reg[8] = data;
		setBank();
		break;
	case 0x8001:
		reg[reg[8]&0x07] = data;
		setBank();
		break;
	case 0xA001:
		if (data & 0x80) {
			reg[ 9] = 1;
			reg[10] = 0;
		} else {
			reg[ 9] = 0;
		}
		break;
	}
}

void Mapper251::setBank() {
	int chr[6];
	int prg[4];

	for (int i = 0; i < 6; i++)
		chr[i] = (reg[i]|(breg[1]<<4)) & ((breg[2]<<4)|0x0F);

	if (reg[8] & 0x80) {
		setVrom1KBank(4, chr[0]);
		setVrom1KBank(5, chr[0]+1);
		setVrom1KBank(6, chr[1]);
		setVrom1KBank(7, chr[1]+1);
		setVrom1KBank(0, chr[2]);
		setVrom1KBank(1, chr[3]);
		setVrom1KBank(2, chr[4]);
		setVrom1KBank(3, chr[5]);
	} else {
		setVrom1KBank(0, chr[0]);
		setVrom1KBank(1, chr[0]+1);
		setVrom1KBank(2, chr[1]);
		setVrom1KBank(3, chr[1]+1);
		setVrom1KBank(4, chr[2]);
		setVrom1KBank(5, chr[3]);
		setVrom1KBank(6, chr[4]);
		setVrom1KBank(7, chr[5]);
	}

	prg[0] = (reg[6]&((breg[3]&0x3F)^0x3F))|(breg[1]);
	prg[1] = (reg[7]&((breg[3]&0x3F)^0x3F))|(breg[1]);
	prg[2] = prg[3] =((breg[3]&0x3F)^0x3F)|(breg[1]);
	prg[2] &= nesRomSize8KB-1;

	if (reg[8] & 0x40)
		setRom8KBanks(prg[2],prg[1],prg[0],prg[3]);
	else
		setRom8KBanks(prg[0],prg[1],prg[2],prg[3]);
}

void Mapper251::extSl() {
	emsl.array("reg", reg, sizeof(reg));
	emsl.array("breg", breg, sizeof(breg));
}
