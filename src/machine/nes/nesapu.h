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
public:
	explicit NesApu(NesCpu *cpu);
	void reset();
	void write(uint address, quint8 data);
	quint8 read(uint address);

	void setChannelUserEnabled(int channelNo, bool on);
	void setSampleRate(int rate);
	void setStereoEnabled(bool on);
	void setMasterVolume(int value);

	void clockFrameCounter(int nCycles);
	const char *grabBuffer(int *size);
	void updateFrameRate();
signals:
	void request_irq_o(bool on);
private:
	NesMachine *machine() const;
	NesCpu *cpu() const;
	quint8 fetchData(uint address);
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
	int frameIrqCounterMax;
	int m_initCounter;
	bool m_initializingHardware;

	static const int SampleBufferSize = 2048*4;
	char sampleBuffer[SampleBufferSize];

	int masterVolume;
	int bufferIndex;
	int sampleRate;
	bool stereo;

	int masterFrameCounter;
	int derivedFrameCounter;
	int countSequence;
	int sampleTimer;
	int frameTime;
	int sampleTimerMax;
	int sampleCount;

	int triValue;

	int smpR1;
	int smpR2;
	int smpTR;
	int smpNS;
	int smpDM;
	int accCount;

	int extraCycles;

	static const int SquareTableSize = 32 * 16;
	static const int TndTableSize = 204 * 16;

	int square_table[SquareTableSize];
	int tnd_table[TndTableSize];

	int sampleValueL;
	int sampleValueR;
	int prevSampleL;
	int prevSampleR;
	int smpAccumL;
	int smpAccumR;
	int smpDiffL;
	int smpDiffR;

	int dcValue;

	int stereoPosLSquare1;
	int stereoPosLSquare2;
	int stereoPosLTriangle;
	int stereoPosLNoise;
	int stereoPosLDMC;
	int stereoPosRSquare1;
	int stereoPosRSquare2;
	int stereoPosRTriangle;
	int stereoPosRNoise;
	int stereoPosRDMC;

	bool m_irqSignal;

	friend class NesApuDMChannel;
};

#endif // NESAPU_H
