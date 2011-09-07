#ifndef MAPPER003_H
#define MAPPER003_H

#include "../nesmapper.h"

class Mapper003 : public NesMapper {
	Q_OBJECT
public:
	void reset();
	void writeHigh(quint16 address, quint8 data);
};

#endif // MAPPER003_H
