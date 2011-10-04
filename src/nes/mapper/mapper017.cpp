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

#include "mapper017.h"
#include <QDataStream>

void Mapper017::reset() {
	NesMapper::reset();

	setRom8KBanks(0, 1, nesRomSize8KB-2, nesRomSize8KB-1);
	if (nesVromSize1KB)
		setVrom8KBank(0);
	irqEnable = 0;
	irqCounter = 0;
}

void Mapper017::writeLow(u16 address, u8 data) {
	switch (address) {
	case 0x42FE:
		if (data & 0x10)
			setMirroring(SingleHigh);
		else
			setMirroring(SingleLow);
		break;
	case 0x42FF:
		if (data & 0x10)
			setMirroring(HorizontalMirroring);
		else
			setMirroring(VerticalMirroring);
		break;
	case 0x4501:
		irqEnable = 0;
		setIrqSignalOut(false);
		break;
	case 0x4502:
		irqCounter = (irqCounter&0xFF00) | (data<<0);
		break;
	case 0x4503:
		irqLatch = (irqCounter&0x00FF) | (data<<8);
		irqCounter = irqLatch;
		irqEnable = 0xFF;
		break;
	case 0x4504:
	case 0x4505:
	case 0x4506:
	case 0x4507:
		setRom8KBank(address&0x07, data);
		break;

	case 0x4510:
	case 0x4511:
	case 0x4512:
	case 0x4513:
	case 0x4514:
	case 0x4515:
	case 0x4516:
	case 0x4517:
		setVrom1KBank(address&0x07, data);
		break;

	default:
		NesMapper::writeLow(address, data);
		break;
	}
}

void Mapper017::writeHigh(u16 address, u8 data) {
	Q_UNUSED(address)
	setRom16KBank(4, (data & 0x3C) >> 2);
	setCram8KBank(data & 0x03);
}

void Mapper017::horizontalSync() {
	if(irqEnable) {
		if (irqCounter >= 0xFFFF-113) {
			setIrqSignalOut(true);
			irqCounter &= 0xFFFF;
		} else {
			irqCounter += 133;
		}
	}
}

#define STATE_SERIALIZE_BUILDER(sl) \
STATE_SERIALIZE_BEGIN_##sl(Mapper017, 1) \
	STATE_SERIALIZE_PARENT_##sl(NesMapper) \
	STATE_SERIALIZE_VAR_##sl(irqEnable) \
	STATE_SERIALIZE_VAR_##sl(irqCounter) \
	STATE_SERIALIZE_VAR_##sl(irqLatch) \
STATE_SERIALIZE_END_##sl(Mapper017)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
