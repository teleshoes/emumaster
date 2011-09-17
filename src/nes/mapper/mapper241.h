#ifndef MAPPER241_H
#define MAPPER241_H

#include "../mapper.h"

class Mapper241 : public NesMapper {

public:
	void reset();

	void writeHigh(u16 address, u8 data);
};

#endif // MAPPER241_H
