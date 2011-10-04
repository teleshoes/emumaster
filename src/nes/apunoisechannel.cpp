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

#include "apunoisechannel.h"
#include <imachine.h>
#include <QDataStream>

NesApuNoiseChannel::NesApuNoiseChannel(int channelNo) :
	NesApuChannel(channelNo) {
	m_accValue = 0;
	m_accCount = 1;
}

void NesApuNoiseChannel::reset() {
	NesApuChannel::reset();
	m_randomBit = 0;
	m_randomMode = 0;
	m_shiftReg = 1;
}

void NesApuNoiseChannel::updateSampleValue() {
	if (lengthStatus())
		sampleValue = m_randomBit * masterVolume();
}

void NesApuNoiseChannel::setFrequency(u8 data) {
	progTimerMax = m_noiseWavelengthLUT[data & 0x0F];
	m_randomMode = data >> 7;
}

int NesApuNoiseChannel::m_noiseWavelengthLUT[16] = {
	0x004, 0x008, 0x010, 0x020,
	0x040, 0x060, 0x080, 0x0A0,
	0x0CA, 0x0FE, 0x17C, 0x1FC,
	0x2FA, 0x3F8, 0x7F2, 0xFE4
};

#define STATE_SERIALIZE_BUILDER(sl) \
STATE_SERIALIZE_BEGIN_##sl(NesApuNoiseChannel, 1) \
	STATE_SERIALIZE_PARENT_##sl(NesApuChannel) \
	STATE_SERIALIZE_VAR_##sl(m_randomBit) \
	STATE_SERIALIZE_VAR_##sl(m_randomMode) \
	STATE_SERIALIZE_VAR_##sl(m_shiftReg) \
	STATE_SERIALIZE_VAR_##sl(m_accValue) \
	STATE_SERIALIZE_VAR_##sl(m_accCount) \
STATE_SERIALIZE_END_##sl(NesApuNoiseChannel)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
