#ifndef MAPPER251_H
#define MAPPER251_H

#include "../mapper.h"

class Mapper251 : public NesMapper {

public:
	void reset();

	void writeLow(u16 address, u8 data);
	void writeHigh(u16 address, u8 data);

	void setBank();

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	u8 reg[11];
	u8 breg[4];
};

#endif // MAPPER251_H
