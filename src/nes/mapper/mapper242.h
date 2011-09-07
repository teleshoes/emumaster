#ifndef MAPPER242_H
#define MAPPER242_H

#include "../nesmapper.h"

class Mapper242 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeHigh(quint16 address, quint8 data);
};

#endif // MAPPER242_H
