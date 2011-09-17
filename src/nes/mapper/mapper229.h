#ifndef MAPPER229_H
#define MAPPER229_H

#include "../mapper.h"

class Mapper229 : public NesMapper {

public:
	void reset();

	void writeHigh(u16 address, u8 data);
};

#endif // MAPPER229_H
