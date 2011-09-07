#ifndef MAPPER062_H
#define MAPPER062_H

#include "../nesmapper.h"

class Mapper062 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeHigh(quint16 address, quint8 data);
};

#endif // MAPPER062_H
