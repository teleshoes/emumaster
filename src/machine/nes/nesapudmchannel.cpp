#include "nesapudmchannel.h"
#include "nesapu.h"

NesApuDMChannel::NesApuDMChannel(NesApu *apu, int channelNo) :
	NesApuChannel(channelNo),
	apu(apu) {
}

void NesApuDMChannel::reset() {
	NesApuChannel::reset();
	hasSample = false;
	irqGenerated = false;

	playMode = Normal;
	dmaFrequency = 0;
	dmaCounter = 0;
	deltaCounter = 0;
	playLength = 0;
	shiftCounter = 0;
	status = 0;
	dacLsb = 0;
	shiftReg = 0;

	playStartAddress = 0;
	playAddress = 0;

	reg0x4012 = 0;
	reg0x4013 = 0;
}

void NesApuDMChannel::write0x4010(quint8 data) {
	int tmp = data >> 6;
	if (tmp & 1)
		tmp &= 1;
	playMode = static_cast<Mode>(tmp);
	if (!(data & 0x80))
		irqGenerated = false;
	dmaFrequency = frequencyLUT[data & 0xF];
}

void NesApuDMChannel::write0x4011(quint8 data) {
	deltaCounter = (data >> 1) & 0x3F;
	dacLsb = data & 1;
	if (userEnable)
		sampleValue = ((deltaCounter << 1) + dacLsb);
}

void NesApuDMChannel::write0x4012(quint8 data) {
	playStartAddress = (data << 6) | 0xC000;
	playAddress = playStartAddress;
	reg0x4012 = data;
}

void NesApuDMChannel::write0x4013(quint8 data) {
	playLength = (data << 4) + 1;
	lengthCounter = playLength;
	reg0x4013 = data;
}

void NesApuDMChannel::write0x4015(quint8 data) {
	if (!((data >> 4) & 1)) {
		// Disable:
		lengthCounter = 0;
	} else {
		// Restart:
		playAddress = playStartAddress;
		lengthCounter = playLength;
	}
	irqGenerated = false;
}

void NesApuDMChannel::updateSampleValue()
{ lengthCounter = playLength; }

void NesApuDMChannel::clock() {
	// only alter DAC value if the sample buffer has data:
	if (hasSample){
		if (shiftReg & 1){
			if (deltaCounter < 0x3F)
				deltaCounter++;
		} else {
			if (deltaCounter > 0x00)
				deltaCounter--;
		}
		sampleValue = (enable ? ((deltaCounter << 1) + dacLsb) : 0);
		shiftReg >>= 1;
	}
	dmaCounter--;
	if (dmaCounter <= 0){
		// no more sample bits.
		hasSample = false;
		endOfSample();
		dmaCounter = 8;
	}
}

void NesApuDMChannel::endOfSample() {
	if (!lengthCounter && playMode == Loop) {
		playAddress = playStartAddress;
		lengthCounter = playLength;
	}
	if (lengthCounter > 0) {
		nextSample();
		if(!lengthCounter && playMode == Irq)
			irqGenerated = true;
	}
}

void NesApuDMChannel::nextSample() {
	shiftReg = apu->fetchData(playAddress);
	lengthCounter--;
	playAddress++;
	if (playAddress > 0xFFFF)
		playAddress = 0x8000;
	hasSample = true;
}

int NesApuDMChannel::frequencyLUT[16] = {
	0xD60, 0xBE0, 0xAA0, 0xA00,
	0x8F0, 0x7F0, 0x710, 0x6B0,
	0x5F0, 0x500, 0x470, 0x400,
	0x350, 0x2A0, 0x240, 0x1B0
};
