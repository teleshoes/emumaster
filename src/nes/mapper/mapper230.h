#ifndef MAPPER230_H
#define MAPPER230_H

#include "../mapper.h"

class Mapper230 : public NesMapper {

public:
	void reset();

	void writeHigh(u16 address, u8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	u8 rom_sw;
};

#endif // MAPPER230_H
