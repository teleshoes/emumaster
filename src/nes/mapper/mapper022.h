#ifndef MAPPER022_H
#define MAPPER022_H

#include "../mapper.h"

class Mapper022 : public NesMapper {

public:
	void reset();
	void writeHigh(u16 address, u8 data);
};

#endif // MAPPER022_H
