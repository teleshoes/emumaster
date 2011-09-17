#ifndef MAPPER233_H
#define MAPPER233_H

#include "../mapper.h"

class Mapper233 : public NesMapper {

public:
	void reset();

	void writeHigh(u16 address, u8 data);
};

#endif // MAPPER233_H
