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

#include "input.h"
#include "nes.h"

static bool nextStrobe;
static u8 padA;
static u8 padB;

static u8 padAReg;
static u8 padBReg;

static const int buttonMapping[8] =
{
	EmuPad::Button_A,
	EmuPad::Button_B,
	EmuPad::Button_Select,
	EmuPad::Button_Start,
	EmuPad::Button_Up,
	EmuPad::Button_Down,
	EmuPad::Button_Left,
	EmuPad::Button_Right
};

static inline void strobe()
{
	padAReg = padA;
	padBReg = padB;
	// TODO expad
}

static inline u8 hostToEmu(int buttons)
{
	Q_ASSERT(pad == 0 || pad == 1);
	u8 result = 0;
	for (int i = 0; i < 8; i++) {
		if (buttons & buttonMapping[i])
			result |= 1 << i;
	}
	return result;
}

void nesPadInit()
{
	padA = 0;
	padB = 0;
	padAReg = 0;
	padBReg = 0;
	nextStrobe = false;
}

void nesPadSetButtons(int pad, int buttons)
{
	Q_ASSERT(pad == 0 || pad == 1);
	if (!pad)
		padA = hostToEmu(buttons);
	else
		padB = hostToEmu(buttons);
}

void nesPadWrite(u16 addr, u8 data)
{
	Q_ASSERT(addr < 2);
	if (addr == 0) {
		if (data & 0x01) {
			nextStrobe = true;
		} else if (nextStrobe) {
			nextStrobe = false;
			strobe();
		}
	} else {
		// TODO expad
	}
}

u8 nesPadRead(u16 addr)
{
	Q_ASSERT(addr < 2);
	u8 data;
	if (addr == 0) {
		data = padAReg & 1;
		padAReg >>= 1;
		// TODO expad
	} else {
		data = padBReg & 1;
		padBReg >>= 1;
		// TODO expad
	}
	return	data;
}
