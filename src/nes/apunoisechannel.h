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

#ifndef NESAPUNOISECHANNEL_H
#define NESAPUNOISECHANNEL_H

#include "apuchannel.h"

class NesApuNoiseChannel : public NesApuChannel {
public:
	explicit NesApuNoiseChannel(int channelNo);
	void reset();

	void setFrequency(u8 data);
	void updateSampleValue();

	int sample();
	void clock(int nCycles);
protected:
	void extSl();
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
