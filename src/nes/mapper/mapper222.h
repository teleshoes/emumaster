#ifndef MAPPER222_H
#define MAPPER222_H

#include "../mapper.h"

class Mapper222 : public NesMapper {

public:
	void reset();

	void writeHigh(u16 address, u8 data);
};

#endif // MAPPER222_H
