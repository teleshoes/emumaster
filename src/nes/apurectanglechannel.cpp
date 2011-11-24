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

#include "apurectanglechannel.h"
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

void NesApuRectangleChannel::setSweep(u8 data) {
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

void NesApuRectangleChannel::extSl() {
	emsl.var("sweepShiftAmount", m_sweepShiftAmount);
	emsl.var("sweepDirection", m_sweepDirection);
	emsl.var("sweepUpdateRate", m_sweepUpdateRate);
	emsl.var("sweepEnable", m_sweepEnable);
	emsl.var("sweepCounter", m_sweepCounter);
	emsl.var("sweepCarry", m_sweepCarry);
	emsl.var("updateSweepPeriod", m_updateSweepPeriod);
	emsl.var("rectangleCounter", m_rectangleCounter);
}
