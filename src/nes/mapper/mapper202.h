#ifndef MAPPER202_H
#define MAPPER202_H

#include "../mapper.h"

class Mapper202 : public NesMapper {

public:
	void reset();

	void writeEx(u16 address, u8 data);
	void writeLow(u16 address, u8 data);
	void writeHigh(u16 address, u8 data);
private:
	void writeSub(u16 address, u8 data);
};

#endif // MAPPER202_H
