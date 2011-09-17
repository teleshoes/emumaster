#ifndef MAPPER013_H
#define MAPPER013_H

#include "../mapper.h"

class Mapper013 : public NesMapper {

public:
	void reset();
	void writeHigh(u16 address, u8 data);
};

#endif // MAPPER013_H
