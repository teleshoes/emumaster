#ifndef MAPPER240_H
#define MAPPER240_H

#include "../nesmapper.h"

class Mapper240 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeLow(quint16 address, quint8 data);
};

#endif // MAPPER240_H
