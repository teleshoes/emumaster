#include "nesapurectanglechannel.h"
#include <imachine.h>
#include <QDataStream>

NesApuRectangleChannel::NesApuRectangleChannel(int channelNo) :
	NesApuChannel(channelNo) {
}

void NesApuRectangleChannel::reset() {
	NesApuChannel::reset();
	m_sweepShiftAmount = 0;
	m_sweepDirection = false;
	m_sweepUpdateRate = 0;
	m_sweepEnable = false;
	m_sweepCounter = 0;
	m_sweepCarry = false;
	m_updateSweepPeriod = false;
	m_rectangleCounter = 0;
}

void NesApuRectangleChannel::setSweep(quint8 data) {
	m_sweepShiftAmount = data & 0x07;
	m_sweepDirection = data & 0x08;
	m_sweepUpdateRate = ((data >> 4) & 0x07) + 1;
	m_sweepEnable = data & 0x80;
	m_updateSweepPeriod = true;
}

void NesApuRectangleChannel::clockSweep() {
	if (--m_sweepCounter <= 0) {
		m_sweepCounter = m_sweepUpdateRate;
		if (m_sweepEnable && m_sweepShiftAmount > 0 && progTimerMax > 7) {
			m_sweepCarry = false;
			if (!m_sweepDirection) {
				progTimerMax += (progTimerMax >> m_sweepShiftAmount);
				if (progTimerMax > 4095){
					progTimerMax = 4095;
					m_sweepCarry = true;
				}
			} else {
				progTimerMax -= ((progTimerMax >> m_sweepShiftAmount) - channelNo());
			}
		}
	}
	if (m_updateSweepPeriod) {
		m_updateSweepPeriod = false;
		m_sweepCounter = m_sweepUpdateRate;
	}
}

void NesApuRectangleChannel::updateSampleValue() {
	if (sampleCondition) {
		if (!m_sweepDirection && (progTimerMax + (progTimerMax >> m_sweepShiftAmount)) > 4095) {
			sampleValue = 0;
		} else {
			sampleValue = masterVolume() * m_dutyLUT[(dutyMode() << 3) + m_rectangleCounter];
		}
	} else {
		sampleValue = 0;
	}
}

int NesApuRectangleChannel::m_dutyLUT[32] = {
	0, 1, 0, 0, 0, 0, 0, 0,
	0, 1, 1, 0, 0, 0, 0, 0,
	0, 1, 1, 1, 1, 0, 0, 0,
	1, 0, 0, 1, 1, 1, 1, 1
};

#define STATE_SERIALIZE_BUILDER(sl) \
	STATE_SERIALIZE_BEGIN_##sl(NesApuRectangleChannel) \
	STATE_SERIALIZE_PARENT_##sl(NesApuChannel) \
	STATE_SERIALIZE_VAR_##sl(m_sweepShiftAmount) \
	STATE_SERIALIZE_VAR_##sl(m_sweepDirection) \
	STATE_SERIALIZE_VAR_##sl(m_sweepUpdateRate) \
	STATE_SERIALIZE_VAR_##sl(m_sweepEnable) \
	STATE_SERIALIZE_VAR_##sl(m_sweepCounter) \
	STATE_SERIALIZE_VAR_##sl(m_sweepCarry) \
	STATE_SERIALIZE_VAR_##sl(m_updateSweepPeriod) \
	STATE_SERIALIZE_VAR_##sl(m_rectangleCounter) \
	STATE_SERIALIZE_END(NesApuRectangleChannel)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
