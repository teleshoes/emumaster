#ifndef NESAPUTRIANGLECHANNEL_H
#define NESAPUTRIANGLECHANNEL_H

#include "nesapuchannel.h"

class NesApuTriangleChannel : public NesApuChannel {
public:
	explicit NesApuTriangleChannel(int channelNo);
	void reset();

	void setLinearCounter(quint8 data);

	void clockLinearCounter();
	void clockProgrammableTimer(int nCycles);
	void clockTriangleGenerator();

	void updateSampleCondition();

	bool linearCounterControl;
	int linearCounterLoadValue;
	int linearCounter;
	int triangleCounter;
};

#endif // NESAPUTRIANGLECHANNEL_H
