#ifndef NESAPUTRIANGLECHANNEL_H
#define NESAPUTRIANGLECHANNEL_H

#include "apuchannel.h"

class NesApuTriangleChannel : public NesApuChannel {
public:
	explicit NesApuTriangleChannel(int channelNo);
	void reset();

	void setLinearCounter(quint8 data);

	void clockLinearCounter();
	void clockProgrammableTimer(int nCycles);
	void clockTriangleGenerator();

	void updateSampleCondition();
	void clock(int nCycles);

	int triangleCounter() const;

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	bool m_linearCounterControl;
	int m_linearCounterLoadValue;
	int m_linearCounter;
	int m_triangleCounter;
};

inline void NesApuTriangleChannel::clock(int nCycles) {
	if (progTimerMax > 0) {
		progTimerCount -= nCycles;
		while (progTimerCount <= 0) {
			progTimerCount += progTimerMax + 1;
			if (m_linearCounter > 0 && lengthCounter > 0) {
				m_triangleCounter++;
				m_triangleCounter &= 0x1F;
				if (isEnabled()) {
					if (m_triangleCounter > 0x0F)
						sampleValue = (m_triangleCounter & 0xF);
					else
						sampleValue = 0xF - (m_triangleCounter & 0xF);
					sampleValue <<= 4;
				}
			}
		}
	}
}

inline int NesApuTriangleChannel::triangleCounter() const
{ return m_triangleCounter; }

#endif // NESAPUTRIANGLECHANNEL_H
