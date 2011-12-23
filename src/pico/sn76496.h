#ifndef SN76496_H
#define SN76496_H

#include <imachine.h>

class SN76496 {
public:
	void init(int clock);
	void update(int *buffer,int length);
	void write(int data);
	void sl(const QString &name);
private:
	void setClock(int clock);
	void setGain(int gain);

	uint updateStep;
	int volTable[16];	// volume table
	int regs[8];	// registers
	int lastReg;	// last register written
	int volume[4];		// volume of voice 0-2 and noise
	uint noise;			// noise generator
	int noiseFB;		// noise feedback mask
	int period[4];
	int count[4];
	int output[4];
	int pad[1];
};

extern SN76496 sn76496;
extern "C" void sn76496Write(int data); // for assembler

#endif // SN76496_H