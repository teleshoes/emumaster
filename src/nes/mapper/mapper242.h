#ifndef MAPPER242_H
#define MAPPER242_H

#include "../mapper.h"

class Mapper242 : public NesMapper {

public:
	void reset();

	void writeHigh(u16 address, u8 data);
};

#endif // MAPPER242_H
