#ifndef MAPPER201_H
#define MAPPER201_H

#include "../mapper.h"

class Mapper201 : public NesMapper {

public:
	void reset();

	void writeHigh(u16 address, u8 data);
};

#endif // MAPPER201_H
