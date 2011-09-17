#ifndef MAPPER066_H
#define MAPPER066_H

#include "../mapper.h"

class Mapper066 : public NesMapper {

public:
	void reset();

	void writeLow(u16 address, u8 data);
	void writeHigh(u16 address, u8 data);
};

#endif // MAPPER066_H
