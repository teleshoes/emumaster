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

#ifndef MAPPER024_H
#define MAPPER024_H

#include "../mapper.h"

class Mapper024 : public NesMapper {

public:
	void reset();

	void writeHigh(u16 address, u8 data);
	void clock(uint cycles);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	u8 irq_enable;
	u8 irq_counter;
	u8 irq_latch;
	int irq_clock;
};

#endif // MAPPER024_H
