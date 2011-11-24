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

#include "mapper226.h"
#include "ppu.h"
#include "disk.h"
#include <imachine.h>
#include <QDataStream>

void Mapper226::reset() {
	NesMapper::reset();

	setRom32KBank(0);

	reg[0] = 0;
	reg[1] = 0;
}

void Mapper226::writeHigh(u16 address, u8 data) {
	if (address & 0x001 )
		reg[1] = data;
	else
		reg[0] = data;

	if (reg[0] & 0x40)
		setMirroring(VerticalMirroring);
	else
		setMirroring(HorizontalMirroring);

	u8 bank = ((reg[0]&0x1E)>>1)|((reg[0]&0x80)>>3)|((reg[1]&0x01)<<5);

	if (reg[0] & 0x20) {
		if (reg[0] & 0x01) {
			setRom8KBank(4, bank*4+2 );
			setRom8KBank(5, bank*4+3 );
			setRom8KBank(6, bank*4+2 );
			setRom8KBank(7, bank*4+3 );
		} else {
			setRom8KBank(4, bank*4+0 );
			setRom8KBank(5, bank*4+1 );
			setRom8KBank(6, bank*4+0 );
			setRom8KBank(7, bank*4+1 );
		}
	} else {
		setRom8KBank(4, bank*4+0 );
		setRom8KBank(5, bank*4+1 );
		setRom8KBank(6, bank*4+2 );
		setRom8KBank(7, bank*4+3 );
	}
}

void Mapper226::extSl() {
	emsl.array("reg", reg, sizeof(reg));
}
