#ifndef MAPPER243_H
#define MAPPER243_H

#include "../mapper.h"

class Mapper243 : public NesMapper {

public:
	void reset();

	void writeLow(u16 address, u8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	u8 reg[4];
};

#endif // MAPPER243_H
