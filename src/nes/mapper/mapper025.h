#ifndef MAPPER025_H
#define MAPPER025_H

#include "../mapper.h"

class Mapper025 : public NesMapper {

public:
	void reset();

	void writeHigh(u16 address, u8 data);
	void clock(uint cycles);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	u8 reg[11];

	u8 irq_enable;
	u8 irq_counter;
	u8 irq_latch;
	u8 irq_occur;
	int irq_clock;
};

#endif // MAPPER025_H
