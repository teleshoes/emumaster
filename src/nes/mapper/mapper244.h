#ifndef MAPPER244_H
#define MAPPER244_H

#include "../mapper.h"

class Mapper244 : public NesMapper {

public:
	void reset();

	void writeHigh(u16 address, u8 data);
};

#endif // MAPPER244_H
