#include "apudmchannel.h"
#include "apu.h"
#include <emu.h>
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

void NesApuDMChannel::extSl() {
	int playMode_ = m_playMode;
	emsl.var("irqGenerated", irqGenerated);
	emsl.var("hasSample", m_hasSample);
	emsl.var("playMode", playMode_);
	emsl.var("dmaFrequency", m_dmaFrequency);
	emsl.var("dmaCounter", m_dmaCounter);
	emsl.var("deltaCounter", m_deltaCounter);
	emsl.var("playLength", m_playLength);
	emsl.var("shiftCounter", m_shiftCounter);
	emsl.var("status", m_status);
	emsl.var("dacLsb", m_dacLsb);
	emsl.var("shiftReg", m_shiftReg);
	emsl.var("playStartAddress", m_playStartAddress);
	emsl.var("playAddress", m_playAddress);
	m_playMode = static_cast<Mode>(playMode_);
}
