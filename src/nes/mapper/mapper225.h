#ifndef MAPPER225_H
#define MAPPER225_H

#include "../nesmapper.h"

class Mapper225 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeHigh(quint16 address, quint8 data);
};

#endif // MAPPER225_H
