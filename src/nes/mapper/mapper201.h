#ifndef MAPPER201_H
#define MAPPER201_H

#include "../nesmapper.h"

class Mapper201 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeHigh(quint16 address, quint8 data);
};

#endif // MAPPER201_H
