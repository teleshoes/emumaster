#ifndef MAPPER246_H
#define MAPPER246_H

#include "../nesmapper.h"

class Mapper246 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeHigh(quint16 address, quint8 data);
};

#endif // MAPPER246_H
