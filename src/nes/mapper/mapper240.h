#ifndef MAPPER240_H
#define MAPPER240_H

#include "../mapper.h"

class Mapper240 : public NesMapper {

public:
	void reset();

	void writeLow(u16 address, u8 data);
};

#endif // MAPPER240_H
