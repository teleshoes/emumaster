#ifndef MAPPER040_H
#define MAPPER040_H

#include "../mapper.h"

class Mapper040 : public NesMapper {

public:
	void reset();
	void writeHigh(u16 address, u8 data);

	void horizontalSync(int scanline);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	u8 irq_enable;
	int irq_line;
};

#endif // MAPPER040_H
