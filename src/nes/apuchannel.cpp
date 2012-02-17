/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "apuchannel.h"
#include <emu.h>
#include <QDataStream>

NesApuChannel::NesApuChannel(int channelNo) :
	m_channelNo(channelNo) {
}

void NesApuChannel::reset() {
	m_enabled = false;

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

void NesApuChannel::setVolumeDecay(u8 data) {
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

void NesApuChannel::setFrequency(u8 data) {
	progTimerMax = (progTimerMax & ~0xFF) | data;
	updateSampleCondition();
}

void NesApuChannel::setLength(u8 data) {
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
	if (m_enabled == on)
		return;
	m_enabled = on;
	if (!on)
		lengthCounter = 0;
	updateSampleCondition();
	updateSampleValue();
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

void NesApuChannel::sl(int i) {
	emsl.begin(QString("apuChannel%1").arg(i));
	emsl.var("envelopeReset", envelopeReset);
	emsl.var("lengthCounterEnable", lengthCounterEnable);
	emsl.var("lengthCounter", lengthCounter);
	emsl.var("channelNo", m_channelNo);
	emsl.var("enabled", m_enabled);
	emsl.var("masterVolume", m_masterVolume);
	emsl.var("volume", m_volume);
	emsl.var("envelopeVolume", m_envelopeVolume);
	emsl.var("envelopeDecayRate", m_envelopeDecayRate);
	emsl.var("envelopeDecayDisable", m_envelopeDecayDisable);
	emsl.var("envelopeDecayLoopEnable", m_envelopeDecayLoopEnable);
	emsl.var("envelopeDecayCounter", m_envelopeDecayCounter);
	emsl.var("dutyMode", m_dutyMode);
	extSl();
	emsl.end();
}
