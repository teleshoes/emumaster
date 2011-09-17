#ifndef MAPPER015_H
#define MAPPER015_H

#include "../mapper.h"

class Mapper015 : public NesMapper {

public:
	void reset();
	void writeHigh(u16 address, u8 data);
};

#endif // MAPPER015_H
