#ifndef MAPPER050_H
#define MAPPER050_H

#include "../mapper.h"

class Mapper050 : public NesMapper {

public:
	void reset();

	void writeEx(u16 address, u8 data);
	void writeLow(u16 address, u8 data);

	void horizontalSync(int scanline);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	u8 irq_enable;
};

#endif // MAPPER050_H
