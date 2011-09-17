#ifndef MAPPER062_H
#define MAPPER062_H

#include "../mapper.h"

class Mapper062 : public NesMapper {

public:
	void reset();

	void writeHigh(u16 address, u8 data);
};

#endif // MAPPER062_H
