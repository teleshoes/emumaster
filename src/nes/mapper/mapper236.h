#ifndef MAPPER236_H
#define MAPPER236_H

#include "../mapper.h"

class Mapper236 : public NesMapper {

public:
	void reset();

	void writeHigh(u16 address, u8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	u8 bank;
	u8 mode;
};

#endif // MAPPER236_H
