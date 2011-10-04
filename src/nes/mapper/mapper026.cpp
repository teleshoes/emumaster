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

#include "mapper026.h"
#include "ppu.h"
#include "disk.h"
#include <QDataStream>

void Mapper026::reset() {
	NesMapper::reset();

	irq_enable = 0;
	irq_counter = 0;
	irq_latch = 0;
	irq_clock = 0;

	setRom8KBanks(0, 1, nesRomSize8KB-2, nesRomSize8KB-1);
	if (nesVromSize1KB)
		setVrom8KBank(0);

	u32 crc = nesDiskCrc;
	if (crc == 0x30e64d03) {	// Esper Dream 2 - Aratanaru Tatakai(J)
		nesPpu.setRenderMethod(NesPpu::PostAllRender);
	}
	if (crc == 0x836cc1ab) {	// Mouryou Senki Madara(J)
		nesPpu.setRenderMethod(NesPpu::PostRender);
	}

	// TODO nes->apu->SelectExSound( 1);
}

void Mapper026::writeHigh(u16 address, u8 data) {
	switch (address & 0xF003) {
	case 0x8000:
		setRom16KBank(4, data);
		break;

	case 0x9000: case 0x9001: case 0x9002: case 0x9003:
	case 0xA000: case 0xA001: case 0xA002: case 0xA003:
	case 0xB000: case 0xB001: case 0xB002:
		address = (address&0xfffc)|((address&1)<<1)|((address&2)>>1);
		// TODO nes->apu->ExWrite( address, data);
		break;

	case 0xB003:
		data = data & 0x7F;
		if (data == 0x08 || data == 0x2C)
			setMirroring(SingleHigh);
		else if (data == 0x20)
			setMirroring(VerticalMirroring);
		else if (data == 0x24)
			setMirroring(HorizontalMirroring);
		else if (data == 0x28)
			setMirroring(SingleLow);
		break;

	case 0xC000:
		setRom8KBank(6, data);
		break;

	case 0xD000:
		setVrom1KBank(0, data);
		break;

	case 0xD001:
		setVrom1KBank(2, data);
		break;

	case 0xD002:
		setVrom1KBank(1, data);
		break;

	case 0xD003:
		setVrom1KBank(3, data);
		break;

	case 0xE000:
		setVrom1KBank(4, data);
		break;

	case 0xE001:
		setVrom1KBank(6, data);
		break;

	case 0xE002:
		setVrom1KBank(5, data);
		break;

	case 0xE003:
		setVrom1KBank(7, data);
		break;

	case 0xF000:
		irq_latch = data;
		break;
	case 0xF001:
		irq_enable = (irq_enable & 0x01) * 3;
		setIrqSignalOut(false);
		break;
	case 0xF002:
		irq_enable = data & 0x03;
		if (irq_enable & 0x02) {
			irq_counter = irq_latch;
			irq_clock = 0;
		}
		setIrqSignalOut(false);
		break;
	}
}

void Mapper026::clock(uint cycles) {
	if (irq_enable & 0x02) {
		if ((irq_clock+=cycles) >= 0x72) {
			irq_clock -= 0x72;
			if (irq_counter >= 0xFF) {
				irq_counter = irq_latch;
				setIrqSignalOut(true);
			} else {
				irq_counter++;
			}
		}
	}
}

#define STATE_SERIALIZE_BUILDER(sl) \
STATE_SERIALIZE_BEGIN_##sl(Mapper026, 1) \
	STATE_SERIALIZE_PARENT_##sl(NesMapper) \
	STATE_SERIALIZE_VAR_##sl(irq_enable) \
	STATE_SERIALIZE_VAR_##sl(irq_counter) \
	STATE_SERIALIZE_VAR_##sl(irq_latch) \
	STATE_SERIALIZE_VAR_##sl(irq_clock) \
STATE_SERIALIZE_END_##sl(Mapper026)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
