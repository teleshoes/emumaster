#ifndef MAPPER200_H
#define MAPPER200_H

#include "../mapper.h"

class Mapper200 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeHigh(quint16 address, quint8 data);
};

#endif // MAPPER200_H
