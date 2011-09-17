#ifndef MAPPER058_H
#define MAPPER058_H

#include "../mapper.h"

class Mapper058 : public NesMapper {

public:
	void reset();

	void writeHigh(u16 address, u8 data);
};

#endif // MAPPER058_H
