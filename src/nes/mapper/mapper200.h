#ifndef MAPPER200_H
#define MAPPER200_H

#include "../mapper.h"

class Mapper200 : public NesMapper {

public:
	void reset();

	void writeHigh(u16 address, u8 data);
};

#endif // MAPPER200_H
