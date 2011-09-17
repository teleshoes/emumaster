#ifndef MAPPER232_H
#define MAPPER232_H

#include "../mapper.h"

class Mapper232 : public NesMapper {

public:
	void reset();

	void writeLow(u16 address, u8 data);
	void writeHigh(u16 address, u8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	u8 reg[2];
};

#endif // MAPPER232_H
