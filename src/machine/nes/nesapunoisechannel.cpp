#include "nesapunoisechannel.h"

NesApuNoiseChannel::NesApuNoiseChannel(int channelNo) :
	NesApuChannel(channelNo) {
	m_accValue = 0;
	m_accCount = 1;
}

void NesApuNoiseChannel::reset() {
	NesApuChannel::reset();
	m_randomBit = 0;
	m_randomMode = 0;
	m_shiftReg = 1;
}

void NesApuNoiseChannel::updateSampleValue() {
	if (lengthStatus())
		sampleValue = m_randomBit * masterVolume();
}

void NesApuNoiseChannel::setFrequency(quint8 data) {
	progTimerMax = m_noiseWavelengthLUT[data & 0x0F];
	m_randomMode = data >> 7;
}

int NesApuNoiseChannel::m_noiseWavelengthLUT[16] = {
	0x004, 0x008, 0x010, 0x020,
	0x040, 0x060, 0x080, 0x0A0,
	0x0CA, 0x0FE, 0x17C, 0x1FC,
	0x2FA, 0x3F8, 0x7F2, 0xFE4
};
