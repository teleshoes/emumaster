#ifndef MAPPER070_H
#define MAPPER070_H

#include "../mapper.h"

class Mapper070 : public NesMapper {

public:
	void reset();
	void writeHigh(u16 address, u8 data);
private:
	u8 patch;
};

#endif // MAPPER070_H
