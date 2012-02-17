/*
	Free for non-commercial use.
	For commercial use, separate licencing terms must be obtained.
	(c) Copyright 2011, elemental
*/

#include "pico.h"

static const int buttonsMapping[] = {
	Emu::PadKey_Up,
	Emu::PadKey_Down,
	Emu::PadKey_Left,
	Emu::PadKey_Right,
	Emu::PadKey_B,
	Emu::PadKey_X,
	Emu::PadKey_A,
	Emu::PadKey_Start,
	Emu::PadKey_R1,
	Emu::PadKey_L1,
	Emu::PadKey_Y,
	Emu::PadKey_Select
};

void PicoEmu::updateInput() {
	for (int pad = 0; pad < 2; pad++) {
		int buttons = 0;
		int hostButtons = padOffset(m_inputData, pad)[0];

		for (uint i = 0; i < sizeof(buttonsMapping); i++) {
			if (hostButtons & buttonsMapping[i])
				buttons |= 1 << i;
		}

		PicoPad[pad] = buttons;
	}
}
