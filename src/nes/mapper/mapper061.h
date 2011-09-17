#ifndef MAPPER061_H
#define MAPPER061_H

#include "../mapper.h"

class Mapper061 : public NesMapper {

public:
	void reset();

	void writeHigh(u16 address, u8 data);
};

#endif // MAPPER061_H
