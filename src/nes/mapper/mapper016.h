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

#ifndef MAPPER016_H
#define MAPPER016_H

#include "../mapper.h"
#include "eeprom.h"

class Mapper016 : public NesMapper {

public:
	void reset();

	u8 readLow(u16 address);
	void writeLow(u16 address, u8 data);
	void writeHigh(u16 address, u8 data);

	void clock(uint cycles);
	void horizontalSync();

	void writeSubA(u16 address, u8 data);
	void writeSubB(u16 address, u8 data);

	void extSl();
private:
	enum {
		IrqClock,
		IrqHSync
	};
	u8 patch;	// For Famicom Jump 2
	u8 eeprom_type;

	u8 reg[3];

	u8 irq_enable;
	int irq_counter;
	int irq_latch;
	u8 irq_type;

	X24C01 x24c01;
	X24C02 x24c02;
};

#endif // MAPPER016_H
