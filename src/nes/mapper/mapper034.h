#ifndef MAPPER034_H
#define MAPPER034_H

#include "../mapper.h"

class Mapper034 : public NesMapper {

public:
	void reset();
	void writeLow(u16 address, u8 data);
	void writeHigh(u16 address, u8 data);
};

#endif // MAPPER034_H
