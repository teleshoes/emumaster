#ifndef MAPPER042_H
#define MAPPER042_H

#include "../mapper.h"

class Mapper042 : public NesMapper {

public:
	void reset();
	void writeHigh(u16 address, u8 data);

	void horizontalSync();

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	u8 irq_enable;
	u8 irq_counter;
};

#endif // MAPPER042_H
