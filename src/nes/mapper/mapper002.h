#ifndef MAPPER002_H
#define MAPPER002_H

#include "../mapper.h"

class Mapper002 : public NesMapper {

public:
	void reset();
	void writeLow(u16 address, u8 data);
	void writeHigh(u16 address, u8 data);
private:
	u8 patch;
	bool hasBattery;
};

#endif // MAPPER002_H
