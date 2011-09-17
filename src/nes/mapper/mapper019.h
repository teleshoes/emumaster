#ifndef MAPPER019_H
#define MAPPER019_H

#include "../mapper.h"

class Mapper019 : public NesMapper {

public:
	void reset();

	u8 readLow(u16 address);
	void writeLow(u16 address, u8 data);
	void writeHigh(u16 address, u8 data);

	void clock(uint cycles);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	u8 patch;
	u8 exsound_enable;

	u8 reg[3];
	u8 exram[128];

	u8 irq_enable;
	u16 irq_counter;
};

#endif // MAPPER019_H
