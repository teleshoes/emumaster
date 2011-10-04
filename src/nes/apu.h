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

#ifndef NESAPU_H
#define NESAPU_H

#include <imachine.h>

class NesApu {
public:
	void init();
	void reset();
	void updateMachineType();

	void write(u16 address, u8 data);
	u8 read(u16 address);

	u8 fetchData(u16 address);

	void clockFrameCounter(int nCycles);
	int fillBuffer(char *stream, int size);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
};

extern NesApu nesApu;

#endif // NESAPU_H
