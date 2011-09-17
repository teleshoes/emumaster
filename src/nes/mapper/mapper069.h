#ifndef MAPPER069_H
#define MAPPER069_H

#include "../mapper.h"

class Mapper069 : public NesMapper {

public:
	void reset();

	void writeHigh(u16 address, u8 data);
	void clock(uint cycles);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	u8 patch;
	u8 reg;
	u8 irq_enable;
	u32 irq_counter;
};

#endif // MAPPER069_H
