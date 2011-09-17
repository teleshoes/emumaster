#ifndef MAPPER043_H
#define MAPPER043_H

#include "../mapper.h"

class Mapper043 : public NesMapper {

public:
	void reset();

	u8 readLow(u16 address);
	void writeEx(u16 address, u8 data);
	void writeLow(u16 address, u8 data);
	void writeHigh(u16 address, u8 data);

	void horizontalSync();

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	u8 irq_enable;
	int irq_counter;
};

#endif // MAPPER043_H
