#ifndef MAPPER041_H
#define MAPPER041_H

#include "../mapper.h"

class Mapper041 : public NesMapper {

public:
	void reset();

	void writeLow(u16 address, u8 data);
	void writeHigh(u16 address, u8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	u8 reg[2];
};

#endif // MAPPER041_H
