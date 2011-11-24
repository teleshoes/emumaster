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

#ifndef NESAPUTRIANGLECHANNEL_H
#define NESAPUTRIANGLECHANNEL_H

#include "apuchannel.h"

class NesApuTriangleChannel : public NesApuChannel {
public:
	explicit NesApuTriangleChannel(int channelNo);
	void reset();

	void setLinearCounter(u8 data);

	void clockLinearCounter();
	void clockProgrammableTimer(int nCycles);
	void clockTriangleGenerator();

	void updateSampleCondition();
	void clock(int nCycles);

	int triangleCounter() const;

protected:
	void extSl();
private:
	bool m_linearCounterControl;
	int m_linearCounterLoadValue;
	int m_linearCounter;
	int m_triangleCounter;
};

inline void NesApuTriangleChannel::clock(int nCycles) {
	if (progTimerMax > 0) {
		progTimerCount -= nCycles;
		while (progTimerCount <= 0) {
			progTimerCount += progTimerMax + 1;
			if (m_linearCounter > 0 && lengthCounter > 0) {
				m_triangleCounter++;
				m_triangleCounter &= 0x1F;
				if (isEnabled()) {
					if (m_triangleCounter > 0x0F)
						sampleValue = (m_triangleCounter & 0xF);
					else
						sampleValue = 0xF - (m_triangleCounter & 0xF);
					sampleValue <<= 4;
				}
			}
		}
	}
}

inline int NesApuTriangleChannel::triangleCounter() const
{ return m_triangleCounter; }

#endif // NESAPUTRIANGLECHANNEL_H
