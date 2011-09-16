#ifndef MAPPER235_H
#define MAPPER235_H

#include "../mapper.h"

class Mapper235 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeHigh(quint16 address, quint8 data);
};

#endif // MAPPER235_H
