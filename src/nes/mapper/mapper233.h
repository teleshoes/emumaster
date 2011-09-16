#ifndef MAPPER233_H
#define MAPPER233_H

#include "../mapper.h"

class Mapper233 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeHigh(quint16 address, quint8 data);
};

#endif // MAPPER233_H
