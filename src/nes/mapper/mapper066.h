#ifndef MAPPER066_H
#define MAPPER066_H

#include "../mapper.h"

class Mapper066 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeLow(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);
};

#endif // MAPPER066_H
