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

	int sweepShiftAmount;
	bool sweepDirection;
	int sweepUpdateRate;
	bool sweepEnable;
	int sweepCounter;
	bool sweepCarry;
	bool updateSweepPeriod;
	int squareCounter;

	static int dutyLUT[32];
};

#endif // NESAPURECTANGLECHANNEL_H
