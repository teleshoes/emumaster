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

#ifndef NESPAD_H
#define NESPAD_H

#include <imachine.h>

class NesPad {
public:
	enum Button {
		A = 0x01,
		B = 0x02,
		Select = 0x04,
		Start = 0x08,
		Up = 0x10,
		Down = 0x20,
		Left = 0x40,
		Right = 0x80
	};
	Q_DECLARE_FLAGS(Buttons, Button)

	void init();
	bool isZapperMode() const;

	void setKeys(int pad, int keys);
	void write(u16 address, u8 data);
	u8 read(u16 address);
private:
	void strobe();
};

inline bool NesPad::isZapperMode() const
{ return false; } // TODO zapper

extern NesPad nesPad;

#endif // NESPAD_H
