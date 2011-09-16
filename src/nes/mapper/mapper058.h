#ifndef MAPPER058_H
#define MAPPER058_H

#include "../mapper.h"

class Mapper058 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeHigh(quint16 address, quint8 data);
};

#endif // MAPPER058_H
