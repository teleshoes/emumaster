#ifndef MAPPER255_H
#define MAPPER255_H

#include "../mapper.h"

class Mapper255 : public NesMapper {

public:
	void reset();

	u8 readLow(u16 address);
	void writeLow(u16 address, u8 data);
	void writeHigh(u16 address, u8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	u8 reg[4];
};

#endif // MAPPER255_H
