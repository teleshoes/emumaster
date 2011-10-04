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

#ifndef MAPPER012_H
#define MAPPER012_H

#include "../mapper.h"

class Mapper012 : public NesMapper {

public:
	void reset();

	void writeLow(u16 address, u8 data);
	u8 readLow(u16 address);
	void writeHigh(u16 address, u8 data);

	void horizontalSync();

	void updatePpuBanks();
	void updateCpuBanks();

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	u32 vb0, vb1;
	u8	reg[8];
	u8	prg0, prg1;
	u8	chr01, chr23, chr4, chr5, chr6, chr7;

	u8	irq_enable;
	u8	irq_counter;
	u8	irq_latch;
	u8	irq_request;
	u8	irq_preset;
	u8	irq_preset_vbl;
};

#endif // MAPPER012_H
