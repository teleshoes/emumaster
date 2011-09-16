#ifndef MAPPER034_H
#define MAPPER034_H

#include "../mapper.h"

class Mapper034 : public NesMapper {
	Q_OBJECT
public:
	void reset();
	void writeLow(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);
};

#endif // MAPPER034_H
