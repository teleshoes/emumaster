#ifndef NESAPUCHANNEL_H
#define NESAPUCHANNEL_H

#include <QtGlobal>

class NesApuChannel {
public:
	explicit NesApuChannel(int channelNo);
	virtual void reset();

	void setVolumeDecay(quint8 data);
	void setLinearCounter(quint8 data);
	void setFrequency(quint8 data);
	void setLength(quint8 data);
	void setEnabled(bool on);
	void setUserEnabled(bool on);

	bool lengthStatus() const;

	void clockLengthCounter();
	void clockEnvelopeDecay();

	void updateMasterVolume();
	virtual void updateSampleCondition();
	virtual void updateSampleValue() {}

	int channelNo;
	bool enable;
	bool userEnable;

	int masterVolume;
	int volume;

	bool lengthCounterEnable;
	int lengthCounter;

	int envelopeVolume;
	int envelopeDecayRate;
	bool envelopeDecayDisable;
	bool envelopeDecayLoopEnable;
	int envelopeDecayCounter;

	union {
		bool envelopeReset;
		bool linearCounterHalt;
	};

	int progTimerMax;
	int progTimerCount;

	int dutyMode;
	bool sampleCondition;
	int sampleValue;

	static int lengthMaxLUT[32];
};

inline bool NesApuChannel::lengthStatus() const
{ return lengthCounter != 0 && enable; }

#endif // NESAPUCHANNEL_H
