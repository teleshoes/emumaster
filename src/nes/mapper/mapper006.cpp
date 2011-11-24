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

#include "mapper006.h"
#include <QDataStream>

void Mapper006::reset() {
	NesMapper::reset();

	setRom8KBanks(0, 1, 14, 15);
	if (nesVromSize1KB)
		setVrom8KBank(0);
	else
		setCram8KBank(0);
	irqEnable = 0;
	irqCounter = 0;
}

void Mapper006::writeLow(u16 address, u8 data) {
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
		irqCounter = (irqCounter&0x00FF) | (data<<8);
		irqEnable = 0xFF;
		setIrqSignalOut(false);
		break;
	default:
		Mapper006::writeLow(address, data);
		break;
	}
}

void Mapper006::writeHigh(u16 address, u8 data) {
	Q_UNUSED(address)
	setRom16KBank(4, (data & 0x3C) >> 2);
	setCram8KBank(data & 0x03);
}

void Mapper006::horizontalSync() {
	if(irqEnable) {
		irqCounter += 133;
		if (irqCounter >= 0xFFFF) {
			irqCounter = 0;
			setIrqSignalOut(true);
		}
	}
}

void Mapper006::extSl() {
	emsl.var("irqEnable", irqEnable);
	emsl.var("irqCounter", irqCounter);
}
