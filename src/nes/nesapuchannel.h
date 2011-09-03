#ifndef NESAPUCHANNEL_H
#define NESAPUCHANNEL_H

#include <QtGlobal>

class NesApuChannel {
public:
	explicit NesApuChannel(int channelNo);
	virtual void reset();

	int channelNo() const;
	void setVolumeDecay(quint8 data);
	int masterVolume() const;
	void setLinearCounter(quint8 data);
	void setFrequency(quint8 data);
	void setLength(quint8 data);
	void setEnabled(bool on);
	bool isEnabled() const;
	void setUserEnabled(bool on);
	bool isUserEnabled() const;

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

	virtual bool save(QDataStream &s);
	virtual bool load(QDataStream &s);
protected:
	union {
		bool envelopeReset;
		bool linearCounterHalt;
	};
	bool lengthCounterEnable;
	int lengthCounter;
private:
	int m_channelNo;
	bool m_enabled;
	bool m_enableLatch;
	bool m_userEnabled;

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
inline bool NesApuChannel::isUserEnabled() const
{ return m_userEnabled; }
inline bool NesApuChannel::lengthStatus() const
{ return lengthCounter != 0 && m_enabled; }
inline int NesApuChannel::dutyMode() const
{ return m_dutyMode; }

#endif // NESAPUCHANNEL_H
