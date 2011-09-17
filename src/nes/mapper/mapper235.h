#ifndef MAPPER235_H
#define MAPPER235_H

#include "../mapper.h"

class Mapper235 : public NesMapper {

public:
	void reset();

	void writeHigh(u16 address, u8 data);
};

#endif // MAPPER235_H
