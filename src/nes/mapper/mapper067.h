#ifndef MAPPER067_H
#define MAPPER067_H

#include "../mapper.h"

class Mapper067 : public NesMapper {

public:
	void reset();

	void writeHigh(u16 address, u8 data);
	void clock(uint cycles);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	u8 irq_enable;
	u8 irq_toggle;
	u32 irq_counter;
};

#endif // MAPPER067_H
