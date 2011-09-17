#ifndef MAPPER225_H
#define MAPPER225_H

#include "../mapper.h"

class Mapper225 : public NesMapper {

public:
	void reset();

	void writeHigh(u16 address, u8 data);
};

#endif // MAPPER225_H
