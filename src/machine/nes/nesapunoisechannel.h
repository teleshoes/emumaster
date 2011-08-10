#ifndef NESAPUNOISECHANNEL_H
#define NESAPUNOISECHANNEL_H

#include "nesapuchannel.h"

class NesApuNoiseChannel : public NesApuChannel {
public:
	explicit NesApuNoiseChannel(int channelNo);
	void reset();

	void setFrequency(quint8 data);
	void updateSampleValue();

	void step(int nCycles);

	int randomBit;
	bool randomMode;
	int shiftReg;
	int accValue;
	int accCount;

	static int m_noiseWavelengthLUT[16];
};

inline void NesApuNoiseChannel::step(int nCycles) {
	if (progTimerCount - nCycles > 0) {
		// do all cycles at once
		progTimerCount -= nCycles;
		accCount       += nCycles;
		Q_ASSERT(accCount != 0);
		accValue       += nCycles * sampleValue;
	} else {
		// slow-step
		for (; nCycles > 0; nCycles--) {
			progTimerCount--;
			if (progTimerCount <= 0 && progTimerMax > 0) {
				shiftReg <<= 1;
				int noiseTmp = shiftReg;
				noiseTmp <<= (randomMode ? 6 : 1);
				noiseTmp ^= shiftReg;
				noiseTmp &= 0x8000;
				if (noiseTmp) {
					shiftReg |= 0x01;
					randomBit = 0;
					sampleValue = 0;
				} else {
					randomBit = 1;
					if (lengthStatus()) {
						sampleValue = masterVolume;
					} else {
						sampleValue = 0;
					}
				}
				progTimerCount += progTimerMax;
			}
			accValue += sampleValue;
			accCount++;
			Q_ASSERT(accCount != 0);
		}
	}
}

#endif // NESAPUNOISECHANNEL_H
