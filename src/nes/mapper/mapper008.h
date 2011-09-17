#ifndef MAPPER008_H
#define MAPPER008_H

#include "../mapper.h"

class Mapper008 : public NesMapper {

public:
	void reset();
	void writeHigh(u16 address, u8 data);
};

#endif // MAPPER008_H
