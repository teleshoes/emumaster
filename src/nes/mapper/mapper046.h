#ifndef MAPPER046_H
#define MAPPER046_H

#include "../mapper.h"

class Mapper046 : public NesMapper {

public:
	void reset();
	void writeLow(u16 address, u8 data);
	void writeHigh(u16 address, u8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	void updateBanks();

	u32 reg[4];
};

#endif // MAPPER046_H
