#ifndef MAPPER057_H
#define MAPPER057_H

#include "../mapper.h"

class Mapper057 : public NesMapper {

public:
	void reset();

	void writeHigh(u16 address, u8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	u8 reg;
};

#endif // MAPPER057_H
