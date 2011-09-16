#ifndef MAPPER231_H
#define MAPPER231_H

#include "../mapper.h"

class Mapper231 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeHigh(quint16 address, quint8 data);
};

#endif // MAPPER231_H
