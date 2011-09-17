#ifndef MAPPER033_H
#define MAPPER033_H

#include "../mapper.h"

class Mapper033 : public NesMapper {

public:
	void reset();

	void writeHigh(u16 address, u8 data);

	void horizontalSync();

	bool save(QDataStream &s);
	bool load(QDataStream &s);

	void updateBanks();
private:
	u8 patch;

	u8 reg[7];

	u8 irq_enable;
	u8 irq_counter;
	u8 irq_latch;
};

#endif // MAPPER033_H
