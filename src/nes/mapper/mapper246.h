#ifndef MAPPER246_H
#define MAPPER246_H

#include "../mapper.h"

class Mapper246 : public NesMapper {

public:
	void reset();

	void writeHigh(u16 address, u8 data);
};

#endif // MAPPER246_H
