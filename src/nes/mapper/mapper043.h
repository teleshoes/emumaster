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

#ifndef MAPPER043_H
#define MAPPER043_H

#include "../mapper.h"

class Mapper043 : public NesMapper {

public:
	void reset();

	u8 readLow(u16 address);
	void writeEx(u16 address, u8 data);
	void writeLow(u16 address, u8 data);
	void writeHigh(u16 address, u8 data);

	void horizontalSync();

	void extSl();
private:
	u8 irq_enable;
	int irq_counter;
};

#endif // MAPPER043_H
