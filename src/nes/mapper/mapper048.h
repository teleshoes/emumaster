#ifndef MAPPER048_H
#define MAPPER048_H

#include "../mapper.h"

class Mapper048 : public NesMapper {

public:
	void reset();
	void writeHigh(u16 address, u8 data);

	void horizontalSync();

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	u8 reg;
	u8 irq_enable;
	u8 irq_counter;
};

#endif // MAPPER048_H
