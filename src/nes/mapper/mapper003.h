#ifndef MAPPER003_H
#define MAPPER003_H

#include "../mapper.h"

class Mapper003 : public NesMapper {

public:
	void reset();
	void writeHigh(u16 address, u8 data);
};

#endif // MAPPER003_H
