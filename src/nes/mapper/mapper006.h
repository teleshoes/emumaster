#ifndef MAPPER006_H
#define MAPPER006_H

#include "../mapper.h"

class Mapper006 : public NesMapper {

public:
	void reset();
	void writeLow(u16 address, u8 data);
	void writeHigh(u16 address, u8 data);

	void horizontalSync(int scanline);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	u8 irqEnable;
	int irqCounter;
};

#endif // MAPPER006_H
