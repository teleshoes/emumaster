#ifndef MAPPER068_H
#define MAPPER068_H

#include "../mapper.h"

class Mapper068 : public NesMapper {

public:
	void reset();

	void writeHigh(u16 address, u8 data);

	void updateBanks();

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	u8 reg[4];
};

#endif // MAPPER068_H
