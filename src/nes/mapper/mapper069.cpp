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

#include "mapper069.h"
#include "disk.h"
#include "ppu.h"
#include <QDataStream>

void Mapper069::reset() {
	NesMapper::reset();

	patch = 0;

	// TODO nes->apu->SelectExSound(32);

	setRom8KBanks(0, 1, nesRomSize8KB-2, nesRomSize8KB-1);

	if (nesVromSize1KB)
		setVrom8KBank(0);

	irq_enable = 0;
	irq_counter = 0;


	u32 crc = nesDiskCrc;

	if (crc == 0xfeac6916)	// Honoo no Toukyuuji - Dodge Danpei 2(J)
		nesEmu.setRenderMethod(NesEmu::TileRender);

	if (crc == 0xad28aef6)	// Dynamite Batman(J) / Dynamite Batman - Return of the Joker(U)
		patch = 1;
}

void Mapper069::writeHigh(u16 address, u8 data) {
	switch (address & 0xE000) {
	case 0x8000:
		reg = data;
		break;

	case 0xA000:
		switch( reg & 0x0F) {
		case 0x00:	case 0x01:
		case 0x02:	case 0x03:
		case 0x04:	case 0x05:
		case 0x06:	case 0x07:
			setVrom1KBank(reg&0x07, data);
			break;
		case 0x08:
			if (!patch && !(data & 0x40)) {
				setRom8KBank(3, data);
			}
			break;
		case 0x09:
			setRom8KBank(4, data);
			break;
		case 0x0A:
			setRom8KBank(5, data);
			break;
		case 0x0B:
			setRom8KBank(6, data);
			break;

		case 0x0C:
			setMirroring(static_cast<NesMirroring>(data & 0x03));
			break;

		case 0x0D:
			irq_enable = data;
			setIrqSignalOut(false);
			break;

		case 0x0E:
			irq_counter = (irq_counter & 0xFF00) | data;
			setIrqSignalOut(false);
			break;

		case 0x0F:
			irq_counter = (irq_counter & 0x00FF) | (data << 8);
			setIrqSignalOut(false);
			break;
		}
		break;

	case 0xC000:
	case 0xE000:
		// TODO nes->apu->ExWrite( address, data);
		break;
	}
}

void Mapper069::clock(uint cycles) {
	if (irq_enable) {
		irq_counter -= cycles;
		if (irq_counter <= 0) {
			setIrqSignalOut(true);
			irq_enable = 0;
			irq_counter = 0xFFFF;
		}
	}
}

void Mapper069::extSl() {
	emsl.var("reg", reg);
	emsl.var("irq_enable", irq_enable);
	emsl.var("irq_counter", irq_counter);
}
