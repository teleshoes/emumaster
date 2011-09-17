#ifndef MAPPER047_H
#define MAPPER047_H

#include "../mapper.h"

class Mapper047 : public NesMapper {

public:
	void reset();

	void writeLow(u16 address, u8 data);
	void writeHigh(u16 address, u8 data);

	void horizontalSync();

	void setBankCpu();
	void setBankPpu();

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	u8 reg[8];
	u8 patch;
	u8 bank;
	u8 prg0, prg1;
	u8 chr01,chr23,chr4,chr5,chr6,chr7;
	u8 irq_enable;
	u8 irq_counter;
	u8 irq_latch;
};

#endif // MAPPER047_H
