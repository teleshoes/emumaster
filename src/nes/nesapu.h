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
	void updateMachineType();
	void reset();

	void write(quint16 address, quint8 data);
	quint8 read(quint16 address);

	void setSampleRate(int rate);

	void clockFrameCounter(int nCycles);
	int fillBuffer(char *stream, int size);

	virtual bool save(QDataStream &s);
	virtual bool load(QDataStream &s);
signals:
	void request_irq_o(bool on);
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

	int m_masterFrameCounter;
	int m_derivedFrameCounter;
	int m_countSequence;

	int m_sampleTimer;
	int m_frameTime;
	int m_sampleTimerMax;

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

	int m_prevSampleL;
	int m_prevSampleR;
	int m_smpAccumL;
	int m_smpAccumR;

	int m_dcValue;

	bool m_irqSignal;

	static const int StereoPosLR1 =  80;
	static const int StereoPosLR2 = 170;
	static const int StereoPosLTR = 100;
	static const int StereoPosLNS = 150;
	static const int StereoPosLDM = 128;
	static const int StereoPosRR1 = 256 - StereoPosLR1;
	static const int StereoPosRR2 = 256 - StereoPosLR2;
	static const int StereoPosRTR = 256 - StereoPosLTR;
	static const int StereoPosRNS = 256 - StereoPosLNS;
	static const int StereoPosRDM = 256 - StereoPosLDM;

	friend class NesApuDMChannel;
};

inline int NesApu::fillBuffer(char *stream, int size) {
	m_bufferIndex = 0;
	int n = qMin(size, m_bufferIndex);
	memcpy(stream, m_sampleBuffer, n);
	return n;
}

#endif // NESAPU_H
