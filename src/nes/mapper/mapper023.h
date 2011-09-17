#ifndef MAPPER023_H
#define MAPPER023_H

#include "../mapper.h"

class Mapper023 : public NesMapper {

public:
	void reset();

	void writeHigh(u16 address, u8 data);
	void clock(uint cycles);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	u16 addrmask;

	u8 reg[9];

	u8 irq_enable;
	u8 irq_counter;
	u8 irq_latch;
	int irq_clock;
};

#endif // MAPPER023_H
