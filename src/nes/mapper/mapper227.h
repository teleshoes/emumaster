#ifndef MAPPER227_H
#define MAPPER227_H

#include "../mapper.h"

class Mapper227 : public NesMapper {

public:
	void reset();

	void writeHigh(u16 address, u8 data);
};

#endif // MAPPER227_H
