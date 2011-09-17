#ifndef NESAPU_H
#define NESAPU_H

#include <imachine.h>

class NesApu {
public:
	void init();
	void reset();
	void updateMachineType();

	void write(u16 address, u8 data);
	u8 read(u16 address);

	u8 fetchData(u16 address);

	void clockFrameCounter(int nCycles);
	int fillBuffer(char *stream, int size);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
};

extern NesApu nesApu;

#endif // NESAPU_H
