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

#ifndef NESAPUDPCMCHANNEL_H
#define NESAPUDPCMCHANNEL_H

class NesApu;
#include "apuchannel.h"

class NesApuDMChannel : public NesApuChannel {
public:
	enum Mode { Normal, Loop, Irq };

	explicit NesApuDMChannel(int channelNo);
	void reset();

	void write0x4010(u8 data);
	void write0x4011(u8 data);
	void write0x4012(u8 data);
	void write0x4013(u8 data);
	void write0x4015(u8 data);

	void updateSampleValue(); // for setEnabled

	void clockDM();
	void endOfSample();
	void nextSample();

	void clock(int nCycles);

	bool irqGenerated;
protected:
	void extSl();
private:
	bool m_hasSample;

	Mode m_playMode;
	int m_dmaFrequency;
	int m_dmaCounter;
	int m_deltaCounter;
	int m_playLength;
	int m_shiftCounter;
	int m_status;
	int m_dacLsb;
	int m_shiftReg;

	uint m_playStartAddress;
	uint m_playAddress;

	static int m_frequencyLUT[16];
};

inline void NesApuDMChannel::clock(int nCycles) {
	if (isEnabled()) {
		m_shiftCounter -= (nCycles << 3);
		while (m_shiftCounter <= 0 && m_dmaFrequency > 0) {
			m_shiftCounter += m_dmaFrequency;
			clockDM();
		}
	}
}

#endif // NESAPUDPCMCHANNEL_H
