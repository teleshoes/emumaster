#ifndef MAPPER231_H
#define MAPPER231_H

#include "../mapper.h"

class Mapper231 : public NesMapper {

public:
	void reset();

	void writeHigh(u16 address, u8 data);
};

#endif // MAPPER231_H
