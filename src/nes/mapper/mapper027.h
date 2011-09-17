#ifndef MAPPER027_H
#define MAPPER027_H

#include "../mapper.h"

class Mapper027 : public NesMapper {

public:
	void reset();

	void writeHigh(u16 address, u8 data);

	void horizontalSync(int scanline);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	u8 reg[9];

	u8 irq_enable;
	u8 irq_counter;
	u8 irq_latch;
	int irq_clock;

	friend class PpuMapper27;
};

#endif // MAPPER027_H
