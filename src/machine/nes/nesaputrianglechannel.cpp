#include "nesaputrianglechannel.h"

NesApuTriangleChannel::NesApuTriangleChannel(int channelNo) :
	NesApuChannel(channelNo) {
}

void NesApuTriangleChannel::reset() {
	NesApuChannel::reset();
	linearCounterHalt = true;
	linearCounterControl = false;
	linearCounterLoadValue = 0;
	linearCounter = 0;
	sampleValue = 0x0F;
}

void NesApuTriangleChannel::setLinearCounter(quint8 data) {
	// new values for linear counter
	linearCounterControl = data & 0x80;
	linearCounterLoadValue = data & 0x7F;
	lengthCounterEnable = !linearCounterControl;
	updateSampleCondition();
}

void NesApuTriangleChannel::clockLinearCounter() {
	if (linearCounterHalt) {
		// load
		linearCounter = linearCounterLoadValue;
		updateSampleCondition();
	} else if (linearCounter > 0) {
		// decrement
		linearCounter--;
		updateSampleCondition();
	}
	if (!linearCounterControl)
		linearCounterHalt = false;
}

void NesApuTriangleChannel::clockProgrammableTimer(int nCycles) {
	if (progTimerMax > 0) {
		progTimerCount += nCycles;
		while (progTimerMax > 0 && progTimerCount >= progTimerMax) {
			progTimerCount -= progTimerMax;
			if (lengthStatus() && linearCounter > 0)
				clockTriangleGenerator();
		}
	}
}

void NesApuTriangleChannel::clockTriangleGenerator(){
	triangleCounter++;
	triangleCounter &= 0x1F;
}

void NesApuTriangleChannel::updateSampleCondition()
{ sampleCondition = (lengthStatus() && progTimerMax > 7 && linearCounter > 0); }
