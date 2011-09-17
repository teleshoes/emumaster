#ifndef MAPPER11_H
#define MAPPER11_H

#include "../mapper.h"

class Mapper011 : public NesMapper {

public:
	void reset();
	void writeHigh(u16 address, u8 data);
};

#endif // MAPPER11_H
