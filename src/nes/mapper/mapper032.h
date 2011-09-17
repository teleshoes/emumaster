#ifndef MAPPER32_H
#define MAPPER32_H

#include "../mapper.h"

class Mapper032 : public NesMapper {

public:
	void reset();
	void writeHigh(u16 address, u8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	u8 patch;
	u8 reg;
};

#endif // MAPPER32_H
