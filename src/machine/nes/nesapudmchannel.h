#ifndef NESAPUDPCMCHANNEL_H
#define NESAPUDPCMCHANNEL_H

class NesApu;
#include "nesapuchannel.h"

class NesApuDMChannel : public NesApuChannel {
public:
	enum Mode { Normal, Loop, Irq };

	explicit NesApuDMChannel(NesApu *m_apu, int channelNo);
	void reset();

	void write0x4010(quint8 data);
	void write0x4011(quint8 data);
	void write0x4012(quint8 data);
	void write0x4013(quint8 data);
	void write0x4015(quint8 data);

	void updateSampleValue(); // for setEnabled

	void clockDM();
	void endOfSample();
	void nextSample();

	void clock(int nCycles);

	bool save(QDataStream &s);
	bool load(QDataStream &s);

	bool irqGenerated;
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

	NesApu *m_apu;

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
