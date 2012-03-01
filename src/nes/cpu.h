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

#ifndef NESCPU_H
#define NESCPU_H

#include "mapper.h"

class NesCpu
{
public:
	void init();

	u32 clock(u32 cycles);
	void dma(u32 cycles);
	u32 ticks() const;

	void sl();

	void apu_irq_i(bool on);
	void irq0_i(bool on);
	void nmi_i(bool on);
	void reset();
	void mapper_irq_i(bool on);

	static const u8 cyclesTable[256];
	static const u8 sizeTable[256];
	static const u8 addressingModeTable[256];
	static const char *nameTable[256];
};

extern NesCpu nesCpu;

#endif // NESCPU_H
