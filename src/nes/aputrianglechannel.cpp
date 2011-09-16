#include "aputrianglechannel.h"
#include <imachine.h>
#include <QDataStream>

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

#define STATE_SERIALIZE_BUILDER(sl) \
STATE_SERIALIZE_BEGIN_##sl(NesApuTriangleChannel, 1) \
	STATE_SERIALIZE_PARENT_##sl(NesApuChannel) \
	STATE_SERIALIZE_VAR_##sl(m_linearCounterControl) \
	STATE_SERIALIZE_VAR_##sl(m_linearCounterLoadValue) \
	STATE_SERIALIZE_VAR_##sl(m_linearCounter) \
	STATE_SERIALIZE_VAR_##sl(m_triangleCounter) \
STATE_SERIALIZE_END_##sl(NesApuTriangleChannel)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
