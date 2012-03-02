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

#include "mapper067.h"
#include "disk.h"
#include "ppu.h"
#include <QDataStream>

void Mapper067::reset() {
	NesMapper::reset();

	irq_enable = 0;
	irq_toggle = 0;
	irq_counter = 0;

	setRom8KBanks(0, 1, nesRomSize8KB-2, nesRomSize8KB-1);

	setVrom4KBank(0, 0);
	setVrom4KBank(4, nesVromSize4KB-1);

	u32 crc = nesDiskCrc;

	if (crc == 0x7f2a04bf) // For Fantasy Zone 2(J)
		nesEmuSetRenderMethod(NesEmu::PreAllRender);
}

void Mapper067::writeHigh(u16 address, u8 data) {
	switch (address & 0xF800) {
	case 0x8800:
		setVrom2KBank(0, data);
		break;
	case 0x9800:
		setVrom2KBank(2, data);
		break;
	case 0xA800:
		setVrom2KBank(4, data);
		break;
	case 0xB800:
		setVrom2KBank(6, data);
		break;

	case 0xC800:
		if (!irq_toggle) {
			irq_counter = (irq_counter&0x00FF) | (data<<8);
		} else {
			irq_counter = (irq_counter&0xFF00) | data;
		}
		irq_toggle ^= 1;
		setIrqSignalOut(false);
		break;
	case 0xD800:
		irq_enable = data & 0x10;
		irq_toggle = 0;
		setIrqSignalOut(false);
		break;

	case 0xE800:
		setMirroring(static_cast<NesMirroring>(data & 0x03));
		break;

	case 0xF800:
		setRom16KBank(4, data);
		break;
	}
}

void Mapper067::clock(uint cycles) {
	if (irq_enable) {
		if ((irq_counter -= cycles) <= 0) {
			irq_enable = 0;
			irq_counter = 0xFFFF;
			setIrqSignalOut(true);
		}
	}
}

void Mapper067::extSl() {
	emsl.var("irq_enable", irq_enable);
	emsl.var("irq_counter", irq_counter);
	emsl.var("irq_toggle", irq_toggle);
}
