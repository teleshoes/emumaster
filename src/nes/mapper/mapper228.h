#ifndef MAPPER228_H
#define MAPPER228_H

#include "../mapper.h"

class Mapper228 : public NesMapper {

public:
	void reset();

	void writeHigh(u16 address, u8 data);
};

#endif // MAPPER228_H
