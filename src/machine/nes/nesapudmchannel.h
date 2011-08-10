#ifndef NESAPUDPCMCHANNEL_H
#define NESAPUDPCMCHANNEL_H

class NesApu;
#include "nesapuchannel.h"

class NesApuDMChannel : public NesApuChannel {
public:
	enum Mode { Normal, Loop, Irq };

	explicit NesApuDMChannel(NesApu *apu, int channelNo);
	void reset();

	void write0x4010(quint8 data);
	void write0x4011(quint8 data);
	void write0x4012(quint8 data);
	void write0x4013(quint8 data);
	void write0x4015(quint8 data);

	void updateSampleValue(); // for setEnabled

	void clock();
	void endOfSample();
	void nextSample();

	bool hasSample;
	bool irqGenerated;

	Mode playMode;
	int dmaFrequency;
	int dmaCounter;
	int deltaCounter;
	int playLength;
	int shiftCounter;
	int status;
	int dacLsb;
	int shiftReg;

	uint playStartAddress;
	uint playAddress;

	quint8 reg0x4012;
	quint8 reg0x4013;

	NesApu *apu;

	static int frequencyLUT[16];
};

#endif // NESAPUDPCMCHANNEL_H
