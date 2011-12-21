#include "machine.h"
#include "pico.h"

static const int buttonsMapping[] = {
	IMachine::PadKey_Up,
	IMachine::PadKey_Down,
	IMachine::PadKey_Left,
	IMachine::PadKey_Right,
	IMachine::PadKey_B,
	IMachine::PadKey_X,
	IMachine::PadKey_A,
	IMachine::PadKey_Start,
	IMachine::PadKey_R1,
	IMachine::PadKey_L1,
	IMachine::PadKey_Y,
	IMachine::PadKey_Select
};

void PicoMachine::updateInput() {
	for (int pad = 0; pad < 2; pad++) {
		int buttons = 0;
		int hostButtons = m_inputData[pad];

		for (uint i = 0; i < sizeof(buttonsMapping); i++) {
			if (hostButtons & buttonsMapping[i])
				buttons |= 1 << i;
		}

		PicoPad[pad] = buttons;
	}
}
