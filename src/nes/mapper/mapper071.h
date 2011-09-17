#ifndef MAPPER071_H
#define MAPPER071_H

#include "../mapper.h"

class Mapper071 : public NesMapper {

public:
	void reset();

	void writeLow(u16 address, u8 data);
	void writeHigh(u16 address, u8 data);
};

#endif // MAPPER071_H
