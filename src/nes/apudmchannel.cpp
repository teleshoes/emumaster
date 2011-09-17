#include "apudmchannel.h"
#include "apu.h"
#include <imachine.h>
#include <QDataStream>

NesApuDMChannel::NesApuDMChannel(int channelNo) :
	NesApuChannel(channelNo) {
}

void NesApuDMChannel::reset() {
	NesApuChannel::reset();
	m_hasSample = false;
	irqGenerated = false;

	m_playMode = Normal;
	m_dmaFrequency = 0;
	m_dmaCounter = 0;
	m_deltaCounter = 0;
	m_playLength = 0;
	m_shiftCounter = 0;
	m_status = 0;
	m_dacLsb = 0;
	m_shiftReg = 0;

	m_playStartAddress = 0;
	m_playAddress = 0;
}

void NesApuDMChannel::write0x4010(u8 data) {
	int tmp = data >> 6;
	if (tmp & 1)
		tmp &= 1;
	m_playMode = static_cast<Mode>(tmp);
	if (!(data & 0x80))
		irqGenerated = false;
	m_dmaFrequency = m_frequencyLUT[data & 0xF];
}

void NesApuDMChannel::write0x4011(u8 data) {
	m_deltaCounter = (data >> 1) & 0x3F;
	m_dacLsb = data & 1;
	sampleValue = ((m_deltaCounter << 1) + m_dacLsb);
}

void NesApuDMChannel::write0x4012(u8 data) {
	m_playStartAddress = (data << 6) | 0xC000;
	m_playAddress = m_playStartAddress;
}

void NesApuDMChannel::write0x4013(u8 data) {
	m_playLength = (data << 4) + 1;
	lengthCounter = m_playLength;
}

void NesApuDMChannel::write0x4015(u8 data) {
	if (!((data >> 4) & 1)) {
		// Disable:
		lengthCounter = 0;
	} else {
		// Restart:
		m_playAddress = m_playStartAddress;
		lengthCounter = m_playLength;
	}
	irqGenerated = false;
}

void NesApuDMChannel::updateSampleValue()
{ lengthCounter = m_playLength; }

void NesApuDMChannel::clockDM() {
	// only alter DAC value if the sample buffer has data:
	if (m_hasSample){
		if (m_shiftReg & 1){
			if (m_deltaCounter < 0x3F)
				m_deltaCounter++;
		} else {
			if (m_deltaCounter > 0x00)
				m_deltaCounter--;
		}
		sampleValue = (isEnabled() ? ((m_deltaCounter << 1) + m_dacLsb) : 0);
		m_shiftReg >>= 1;
	}
	m_dmaCounter--;
	if (m_dmaCounter <= 0){
		// no more sample bits.
		m_hasSample = false;
		endOfSample();
		m_dmaCounter = 8;
	}
}

void NesApuDMChannel::endOfSample() {
	if (!lengthCounter && m_playMode == Loop) {
		m_playAddress = m_playStartAddress;
		lengthCounter = m_playLength;
	}
	if (lengthCounter > 0) {
		nextSample();
		if(!lengthCounter && m_playMode == Irq)
			irqGenerated = true;
	}
}

void NesApuDMChannel::nextSample() {
	m_shiftReg = nesApu.fetchData(m_playAddress);
	lengthCounter--;
	m_playAddress++;
	if (m_playAddress > 0xFFFF)
		m_playAddress = 0x8000;
	m_hasSample = true;
}

int NesApuDMChannel::m_frequencyLUT[16] = {
	0xD60, 0xBE0, 0xAA0, 0xA00,
	0x8F0, 0x7F0, 0x710, 0x6B0,
	0x5F0, 0x500, 0x470, 0x400,
	0x350, 0x2A0, 0x240, 0x1B0
};

#define STATE_SERIALIZE_BUILDER(sl) \
STATE_SERIALIZE_BEGIN_##sl(NesApuDMChannel, 1) \
	STATE_SERIALIZE_PARENT_##sl(NesApuChannel) \
	int playMode_ = m_playMode; \
	STATE_SERIALIZE_VAR_##sl(irqGenerated) \
	STATE_SERIALIZE_VAR_##sl(m_hasSample) \
	STATE_SERIALIZE_VAR_##sl(playMode_) \
	STATE_SERIALIZE_VAR_##sl(m_dmaFrequency) \
	STATE_SERIALIZE_VAR_##sl(m_dmaCounter) \
	STATE_SERIALIZE_VAR_##sl(m_deltaCounter) \
	STATE_SERIALIZE_VAR_##sl(m_playLength) \
	STATE_SERIALIZE_VAR_##sl(m_shiftCounter) \
	STATE_SERIALIZE_VAR_##sl(m_status) \
	STATE_SERIALIZE_VAR_##sl(m_dacLsb) \
	STATE_SERIALIZE_VAR_##sl(m_shiftReg) \
	STATE_SERIALIZE_VAR_##sl(m_playStartAddress) \
	STATE_SERIALIZE_VAR_##sl(m_playAddress) \
	m_playMode = static_cast<Mode>(playMode_); \
STATE_SERIALIZE_END_##sl(NesApuDMChannel)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
