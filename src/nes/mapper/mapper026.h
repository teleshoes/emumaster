#ifndef MAPPER026_H
#define MAPPER026_H

#include "../mapper.h"

class Mapper026 : public NesMapper {

public:
	void reset();

	void writeHigh(u16 address, u8 data);
	void clock(uint cycles);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	u8 irq_enable;
	u8 irq_counter;
	u8 irq_latch;
	int irq_clock;
};

#endif // MAPPER026_H
