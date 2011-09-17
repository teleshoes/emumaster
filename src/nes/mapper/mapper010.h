#ifndef MAPPER010_H
#define MAPPER010_H

#include "../mapper.h"

class Mapper010 : public NesMapper {

public:
	void reset();
	void writeHigh(u16 address, u8 data);

	void characterLatch(u16 address);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	u8 reg[4];
	u8 latch_a, latch_b;
};

#endif // MAPPER010_H
