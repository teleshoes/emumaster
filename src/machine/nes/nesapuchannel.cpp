#include "nesapuchannel.h"

NesApuChannel::NesApuChannel(int channelNo) :
	m_channelNo(channelNo) {
}

void NesApuChannel::reset() {
	m_enabled = false;
	m_userEnabled = true;

	m_masterVolume = 0;
	m_volume = 0;

	lengthCounterEnable = false;
	lengthCounter = 0;

	m_envelopeVolume = 0;
	m_envelopeDecayRate = 0;
	m_envelopeDecayDisable = false;
	m_envelopeDecayLoopEnable = false;
	m_envelopeDecayCounter = 0;
	envelopeReset = false;

	progTimerMax = 0;
	progTimerCount = 0;

	m_dutyMode = 0;
	sampleCondition = false;
	sampleValue = 0;
}

void NesApuChannel::setVolumeDecay(quint8 data) {
	m_volume = data & 0x0F;
	m_envelopeDecayRate = (data & 0x0F) + 1;
	m_envelopeDecayDisable = data & 0x10;
	lengthCounterEnable = !(data & 0x20);
	m_envelopeDecayLoopEnable = data & 0x20;
	m_dutyMode = (data >> 6) & 0x03;
	updateMasterVolume();
	updateSampleCondition();
}

void NesApuChannel::updateMasterVolume()
{ m_masterVolume = m_envelopeDecayDisable ? m_volume : m_envelopeVolume; }

void NesApuChannel::setFrequency(quint8 data) {
	progTimerMax = (progTimerMax & ~0xFF) | data;
	updateSampleCondition();
}

void NesApuChannel::setLength(quint8 data) {
	progTimerMax = (progTimerMax & 0xFF) | ((data & 0x07) << 8);
	if (m_enabled)
		lengthCounter = m_lengthMaxLUT[data >> 3];
	envelopeReset = true;
	updateSampleCondition();
}

int NesApuChannel::m_lengthMaxLUT[32] = {
	0x0A, 0xFE, 0x14, 0x02, 0x28, 0x04, 0x50, 0x06, 0xA0, 0x08, 0x3C, 0x0A, 0x0E, 0x0C, 0x1A, 0x0E,
	0x0C, 0x10, 0x18, 0x12, 0x30, 0x14, 0x60, 0x16, 0xC0, 0x18, 0x48, 0x1A, 0x10, 0x1C, 0x20, 0x1E
};

void NesApuChannel::setEnabled(bool on){
	on &= m_userEnabled;
	if (m_enabled == on)
		return;
	m_enabled = on;
	if (!on)
		lengthCounter = 0;
	updateSampleCondition();
	updateSampleValue();
}

void NesApuChannel::setUserEnabled(bool on) {
	m_userEnabled = on;
	setEnabled(m_enabled);
}

void NesApuChannel::updateSampleCondition()
{ sampleCondition = (lengthStatus() && progTimerMax > 7); }

void NesApuChannel::clockLengthCounter() {
	if (lengthCounterEnable && lengthCounter > 0){
		lengthCounter--;
		if (!lengthCounter) {
			updateSampleCondition();
			updateSampleValue();
		}
	}
}

void NesApuChannel::clockEnvelopeDecay() {
	if (envelopeReset) {
		envelopeReset = false;
		m_envelopeDecayCounter = m_envelopeDecayRate;
		m_envelopeVolume = 0x0F;
	} else if (--m_envelopeDecayCounter <= 0) {
		m_envelopeDecayCounter = m_envelopeDecayRate;
		if (m_envelopeVolume > 0)
			m_envelopeVolume--;
		else
			m_envelopeVolume = (m_envelopeDecayLoopEnable ? 0x0F : 0x00);
	}
	updateMasterVolume();
	updateSampleCondition();
	updateSampleValue();
}
