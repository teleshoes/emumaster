#ifndef MAPPER254_H
#define MAPPER254_H

#include "../mapper.h"

class Mapper254 : public NesMapper {

public:
	void reset();
	u8 readLow(u16 address);
	void writeLow(u16 address, u8 data);
	void writeHigh(u16 address, u8 data);

	void horizontalSync();

	void setBankCpu();
	void setBankPpu();

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	u8 reg[8];
	u8 prg0, prg1;
	u8 chr01, chr23, chr4, chr5, chr6, chr7;

	u8 irq_type;
	u8 irq_enable;
	u8 irq_counter;
	u8 irq_latch;
	u8 irq_request;
	u8 protectflag;
};

#endif // MAPPER254_H
