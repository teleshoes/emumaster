#ifndef MAPPER228_H
#define MAPPER228_H

#include "../mapper.h"

class Mapper228 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeHigh(quint16 address, quint8 data);
};

#endif // MAPPER228_H
