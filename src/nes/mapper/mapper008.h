#ifndef MAPPER008_H
#define MAPPER008_H

#include "../nesmapper.h"

class Mapper008 : public NesMapper {
	Q_OBJECT
public:
	void reset();
	void writeHigh(quint16 address, quint8 data);
};

#endif // MAPPER008_H
