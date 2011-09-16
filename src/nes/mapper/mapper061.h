#ifndef MAPPER061_H
#define MAPPER061_H

#include "../mapper.h"

class Mapper061 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeHigh(quint16 address, quint8 data);
};

#endif // MAPPER061_H
