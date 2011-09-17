#ifndef MAPPER226_H
#define MAPPER226_H

#include "../mapper.h"

class Mapper226 : public NesMapper {

public:
	void reset();

	void writeHigh(u16 address, u8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	u8 reg[2];
};

#endif // MAPPER226_H
