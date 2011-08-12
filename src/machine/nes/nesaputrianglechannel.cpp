#include "nesaputrianglechannel.h"

NesApuTriangleChannel::NesApuTriangleChannel(int channelNo) :
	NesApuChannel(channelNo) {
}

void NesApuTriangleChannel::reset() {
	NesApuChannel::reset();
	linearCounterHalt = true;
	m_linearCounterControl = false;
	m_linearCounterLoadValue = 0;
	m_linearCounter = 0;
	sampleValue = 0x0F;
}

void NesApuTriangleChannel::setLinearCounter(quint8 data) {
	// new values for linear counter
	m_linearCounterControl = data & 0x80;
	m_linearCounterLoadValue = data & 0x7F;
	lengthCounterEnable = !m_linearCounterControl;
	updateSampleCondition();
}

void NesApuTriangleChannel::clockLinearCounter() {
	if (linearCounterHalt) {
		// load
		m_linearCounter = m_linearCounterLoadValue;
		updateSampleCondition();
	} else if (m_linearCounter > 0) {
		// decrement
		m_linearCounter--;
		updateSampleCondition();
	}
	if (!m_linearCounterControl)
		linearCounterHalt = false;
}

void NesApuTriangleChannel::clockProgrammableTimer(int nCycles) {
	if (progTimerMax > 0) {
		progTimerCount += nCycles;
		while (progTimerMax > 0 && progTimerCount >= progTimerMax) {
			progTimerCount -= progTimerMax;
			if (lengthStatus() && m_linearCounter > 0)
				clockTriangleGenerator();
		}
	}
}

void NesApuTriangleChannel::clockTriangleGenerator(){
	m_triangleCounter++;
	m_triangleCounter &= 0x1F;
}

void NesApuTriangleChannel::updateSampleCondition()
{ sampleCondition = (lengthStatus() && progTimerMax > 7 && m_linearCounter > 0); }
