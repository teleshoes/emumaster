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

#ifndef NESAPUCHANNEL_H
#define NESAPUCHANNEL_H

#include <emu.h>

class NesApuChannel {
public:
	explicit NesApuChannel(int channelNo);
	virtual void reset();

	int channelNo() const;
	void setVolumeDecay(u8 data);
	int masterVolume() const;
	void setLinearCounter(u8 data);
	void setFrequency(u8 data);
	void setLength(u8 data);
	void setEnabled(bool on);
	bool isEnabled() const;

	bool lengthStatus() const;
	int dutyMode() const;

	void clockLengthCounter();
	void clockEnvelopeDecay();

	void updateMasterVolume();
	virtual void updateSampleCondition();
	virtual void updateSampleValue() {}

	bool sampleCondition;
	int sampleValue;

	int progTimerMax;
	int progTimerCount;

	void sl(int i);
protected:
	virtual void extSl() = 0;

	union {
		bool envelopeReset;
		bool linearCounterHalt;
	};
	bool lengthCounterEnable;
	int lengthCounter;
private:
	int m_channelNo;
	bool m_enabled;

	int m_masterVolume;
	int m_volume;

	int m_envelopeVolume;
	int m_envelopeDecayRate;
	bool m_envelopeDecayDisable;
	bool m_envelopeDecayLoopEnable;
	int m_envelopeDecayCounter;

	int m_dutyMode;

	static int m_lengthMaxLUT[32];
};

inline int NesApuChannel::channelNo() const
{ return m_channelNo; }
inline int NesApuChannel::masterVolume() const
{ return m_masterVolume; }
inline bool NesApuChannel::isEnabled() const
{ return m_enabled; }
inline bool NesApuChannel::lengthStatus() const
{ return lengthCounter != 0 && m_enabled; }
inline int NesApuChannel::dutyMode() const
{ return m_dutyMode; }

#endif // NESAPUCHANNEL_H
