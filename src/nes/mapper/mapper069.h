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

#ifndef MAPPER069_H
#define MAPPER069_H

#include "../mapper.h"

class Mapper069 : public NesMapper {

public:
	void reset();

	void writeHigh(u16 address, u8 data);
	void clock(uint cycles);

	void extSl();
private:
	u8 patch;
	u8 reg;
	u8 irq_enable;
	s32 irq_counter;
};

#endif // MAPPER069_H
