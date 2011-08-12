#ifndef NESAPUNOISECHANNEL_H
#define NESAPUNOISECHANNEL_H

#include "nesapuchannel.h"

class NesApuNoiseChannel : public NesApuChannel {
public:
	explicit NesApuNoiseChannel(int channelNo);
	void reset();

	void setFrequency(quint8 data);
	void updateSampleValue();

	int sample();
	void clock(int nCycles);
private:
	int m_randomBit;
	bool m_randomMode;
	int m_shiftReg;
	int m_accValue;
	int m_accCount;

	static int m_noiseWavelengthLUT[16];
};

inline int NesApuNoiseChannel::sample() {
	int result = (m_accValue << 4) / m_accCount;
	m_accValue = result >> 4;
	m_accCount = 1;
	return result;
}

inline void NesApuNoiseChannel::clock(int nCycles) {
	Q_ASSERT(nCycles > 0);
	if (progTimerCount - nCycles > 0) {
		// do all cycles at once
		progTimerCount -= nCycles;
		m_accCount += nCycles;
		m_accValue += nCycles * sampleValue;
	} else {
		// slow-step
		for (; nCycles > 0; nCycles--) {
			progTimerCount--;
			if (progTimerCount <= 0 && progTimerMax > 0) {
				m_shiftReg <<= 1;
				int noiseTmp = m_shiftReg;
				noiseTmp <<= (m_randomMode ? 6 : 1);
				noiseTmp ^= m_shiftReg;
				noiseTmp &= 0x8000;
				if (noiseTmp) {
					m_shiftReg |= 0x01;
					m_randomBit = 0;
					sampleValue = 0;
				} else {
					m_randomBit = 1;
					sampleValue = (lengthStatus() ? masterVolume() : 0);
				}
				progTimerCount += progTimerMax;
			}
			m_accValue += sampleValue;
			m_accCount++;
		}
	}
}

#endif // NESAPUNOISECHANNEL_H
