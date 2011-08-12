#ifndef NESAPURECTANGLECHANNEL_H
#define NESAPURECTANGLECHANNEL_H

#include "nesapuchannel.h"

class NesApuRectangleChannel : public NesApuChannel {
public:
	explicit NesApuRectangleChannel(int channelNo);
	void reset();

	void setSweep(quint8 data);

	void clockSweep();
	void updateSampleValue();

	void clock(int nCycles);
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
