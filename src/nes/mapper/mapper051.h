#ifndef MAPPER051_H
#define MAPPER051_H

#include "../mapper.h"

class Mapper051 : public NesMapper {

public:
	void reset();

	void writeLow(u16 address, u8 data);
	void writeHigh(u16 address, u8 data);

	void updateBanks();

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	u32 mode;
	u32 bank;
};

#endif // MAPPER051_H
