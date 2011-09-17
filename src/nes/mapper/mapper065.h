#ifndef MAPPER065_H
#define MAPPER065_H

#include "../mapper.h"

class Mapper065 : public NesMapper {

public:
	void reset();

	void writeHigh(u16 address, u8 data);
	void clock(uint cycles);

	void horizontalSync();

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	u8 patch;
	u8 irq_enable;
	u32 irq_counter;
	u32 irq_latch;
};

#endif // MAPPER065_H
