#ifndef MAPPER060_H
#define MAPPER060_H

#include "../mapper.h"

class Mapper060 : public NesMapper {

public:
	void reset();

	void writeHigh(u16 address, u8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	u8 patch;
	u8 game_sel;
};

#endif // MAPPER060_H
