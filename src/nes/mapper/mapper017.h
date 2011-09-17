#ifndef MAPPER017_H
#define MAPPER017_H

#include "../mapper.h"

class Mapper017 : public NesMapper {

public:
	void reset();
	void writeLow(u16 address, u8 data);
	void writeHigh(u16 address, u8 data);

	void horizontalSync();

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	u8 irqEnable;
	int irqCounter;
	int irqLatch;
};

#endif // MAPPER017_H
