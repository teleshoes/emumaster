#ifndef NESAPU_H
#define NESAPU_H

class NesMachine;
class NesCpu;
#include "nesapurectanglechannel.h"
#include "nesaputrianglechannel.h"
#include "nesapunoisechannel.h"
#include "nesapudmchannel.h"
#include <QObject>

class NesApu : public QObject {
    Q_OBJECT
	Q_PROPERTY(bool rectangle1Enable READ isRectangle1Enabled WRITE setRectangle1Enabled NOTIFY channelUserEnableChanged)
	Q_PROPERTY(bool rectangle2Enable READ isRectangle2Enabled WRITE setRectangle2Enabled NOTIFY channelUserEnableChanged)
	Q_PROPERTY(bool triangleEnable READ isTriangleEnabled WRITE setTriangleEnabled NOTIFY channelUserEnableChanged)
	Q_PROPERTY(bool noiseEnable READ isNoiseEnabled WRITE setNoiseEnabled NOTIFY channelUserEnableChanged)
	Q_PROPERTY(bool dmcEnable READ isDmcEnabled WRITE setDmcEnabled NOTIFY channelUserEnableChanged)
public:
	explicit NesApu(NesCpu *cpu);
	void updateMachineType();
	void reset();

	void write(quint16 address, quint8 data);
	quint8 read(quint16 address);

	bool isChannelUserEnabled(int channelNo) const;
	void setChannelUserEnabled(int channelNo, bool on);

	bool isRectangle1Enabled() const;
	void setRectangle1Enabled(bool on);

	bool isRectangle2Enabled() const;
	void setRectangle2Enabled(bool on);

	bool isTriangleEnabled() const;
	void setTriangleEnabled(bool on);

	bool isNoiseEnabled() const;
	void setNoiseEnabled(bool on);

	bool isDmcEnabled() const;
	void setDmcEnabled(bool on);

	void setSampleRate(int rate);
	void setStereoEnabled(bool on);

	void clockFrameCounter(int nCycles);
	const char *grabBuffer(int *size);
signals:
	void request_irq_o(bool on);
	void channelUserEnableChanged();
private:
	NesMachine *machine() const;
	NesCpu *cpu() const;
	quint8 fetchData(quint16 address);
	void updateFrameRate();
	void initDACLUTs();
	void updateIrqSignal();
	void sample();
	void frameCounterTick();
	void accSample(int cycles);

	NesApuRectangleChannel	m_r1ch;
	NesApuRectangleChannel	m_r2ch;
	NesApuTriangleChannel	m_trch;
	NesApuNoiseChannel		m_nsch;
	NesApuDMChannel			m_dmch;

	bool m_frameIrqGenerated;
	bool m_frameIrqEnable;
	int m_frameIrqCounterMax;
	int m_initCounter;
	bool m_initializingHardware;

	static const int SampleBufferSize = 2048*4;
	char m_sampleBuffer[SampleBufferSize];

	int m_bufferIndex;
	int m_sampleRate;
	bool m_stereo;

	int m_masterFrameCounter;
	int m_derivedFrameCounter;
	int m_countSequence;
	int m_sampleTimer;
	int m_frameTime;
	int m_sampleTimerMax;
	int m_sampleCount;

	int m_triValue;

	int m_smpR1;
	int m_smpR2;
	int m_smpTR;
	int m_smpNS;
	int m_smpDM;
	int m_accCount;

	int m_extraCycles;

	static const int RectangleLUTSize = 32 * 16;
	static const int TriangleLUTSize = 204 * 16;

	int m_rectangleLUT[RectangleLUTSize];
	int m_triangleLUT[TriangleLUTSize];

	int m_sampleValueL;
	int m_sampleValueR;
	int m_prevSampleL;
	int m_prevSampleR;
	int m_smpAccumL;
	int m_smpAccumR;
	int m_smpDiffL;
	int m_smpDiffR;

	int m_dcValue;

	int m_stereoPosLR1;
	int m_stereoPosLR2;
	int m_stereoPosLTR;
	int m_stereoPosLNS;
	int m_stereoPosLDM;
	int m_stereoPosRR1;
	int m_stereoPosRR2;
	int m_stereoPosRTR;
	int m_stereoPosRNS;
	int m_stereoPosRDM;

	bool m_irqSignal;

	friend class NesApuDMChannel;
};

inline const char *NesApu::grabBuffer(int *size) {
	*size = m_bufferIndex;
	m_bufferIndex = 0;
	return m_sampleBuffer;
}

#endif // NESAPU_H
