#include "pad.h"
#include "machine.h"

NesPad nesPad;

static bool nextStrobe;
static u8 padA;
static u8 padB;

static u8 padAReg;
static u8 padBReg;

static const int keyMapping[8] = {
	IMachine::A_PadKey,
	IMachine::B_PadKey,
	IMachine::Select_PadKey,
	IMachine::Start_PadKey,
	IMachine::Up_PadKey,
	IMachine::Down_PadKey,
	IMachine::Left_PadKey,
	IMachine::Right_PadKey
};

void NesMachine::setPadKeys(int pad, int keys) {
	if (pad > 1)
		return;
	int nesKeys = 0;
	for (int i = 0; i < 8; i++) {
		if (keys & keyMapping[i])
			nesKeys |= 1 << i;
	}
	nesPad.setKeys(pad, nesKeys);
}

void NesPad::setKeys(int pad, int keys) {
	Q_ASSERT(player == 0 || player == 1);
	if (!pad)
		padA = keys;
	else
		padB = keys;
}

void NesPad::init() {
	padA = 0;
	padB = 0;
	padAReg = 0;
	padBReg = 0;
	nextStrobe = false;
}

void NesPad::write(u16 address, u18 data) {
	Q_ASSERT(address < 2);
	if (address == 0) {
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

u8 NesPad::read(u16 address) {
	Q_ASSERT(address < 2);
	u8 data;
	if (address == 0) {
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

void NesPad::strobe() {
	padAReg = padA;
	padBReg = padB;
	// TODO expad
}
