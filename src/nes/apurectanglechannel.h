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

#ifndef NESAPURECTANGLECHANNEL_H
#define NESAPURECTANGLECHANNEL_H

#include "apuchannel.h"

class NesApuRectangleChannel : public NesApuChannel {
public:
	explicit NesApuRectangleChannel(int channelNo);
	void reset();

	void setSweep(u8 data);

	void clockSweep();
	void updateSampleValue();

	void clock(int nCycles);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	int m_sweepShiftAmount;
	bool m_sweepDirection;
	int m_sweepUpdateRate;
	bool m_sweepEnable;
	int m_sweepCounter;
	bool m_sweepCarry;
	bool m_updateSweepPeriod;
	int m_rectangleCounter;

	static int m_dutyLUT[32];
};

inline void NesApuRectangleChannel::clock(int nCycles) {
	progTimerCount -= nCycles;
	if (progTimerCount <= 0) {
		progTimerCount += (progTimerMax + 1) << 1;
		m_rectangleCounter++;
		m_rectangleCounter &= 0x7;
		updateSampleValue();
	}
}

#endif // NESAPURECTANGLECHANNEL_H
