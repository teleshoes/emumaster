#ifndef MAPPER018_H
#define MAPPER018_H

#include "../mapper.h"

class Mapper018 : public NesMapper {

public:
	void reset();

	void writeHigh(u16 address, u8 data);

	void clock(uint cycles);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	u8 reg[11];

	u8 irq_enable;
	u8 irq_mode;
	int irq_latch;
	int irq_counter;
};

#endif // MAPPER018_H
