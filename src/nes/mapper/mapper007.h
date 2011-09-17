#ifndef MAPPER007_H
#define MAPPER007_H

#include "../mapper.h"

class Mapper007 : public NesMapper {

public:
	void reset();
	void writeHigh(u16 address, u8 data);
private:
	u8 patch;
};

#endif // MAPPER007_H
