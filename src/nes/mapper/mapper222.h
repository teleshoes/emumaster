#ifndef MAPPER222_H
#define MAPPER222_H

#include "../mapper.h"

class Mapper222 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeHigh(quint16 address, quint8 data);
};

#endif // MAPPER222_H
