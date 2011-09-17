#ifndef MAPPER045_H
#define MAPPER045_H

#include "../mapper.h"

class Mapper045 : public NesMapper {

public:
	void reset();

	void writeLow(u16 address, u8 data);
	void writeHigh(u16 address, u8 data);

	void horizontalSync();

	void setBankCpu(uint page, uint bank);
	void setBankPpu();

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	u8 patch;
	u8 reg[8];
	u8 prg0, prg1, prg2, prg3;
	u8 chr0, chr1, chr2, chr3, chr4, chr5, chr6, chr7;
	u8 p[4];
	s32 c[8];
	u8 irq_enable;
	u8 irq_counter;
	u8 irq_latch;
	u8 irq_latched;
	u8 irq_reset;
};

#endif // MAPPER045_H
