#ifndef MAPPER064_H
#define MAPPER064_H

#include "../mapper.h"

class Mapper064 : public NesMapper {

public:
	void reset();

	void writeHigh(u16 address, u8 data);
	void clock(uint cycles);

	void horizontalSync(int scanline);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	u8 reg[3];
	u8 irq_enable;
	u8 irq_mode;
	s32 irq_counter;
	s32 irq_counter2;
	u8 irq_latch;
	u8 irq_reset;
};

#endif // MAPPER064_H
