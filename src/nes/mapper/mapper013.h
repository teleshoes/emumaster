#ifndef MAPPER013_H
#define MAPPER013_H

#include "../nesmapper.h"

class Mapper013 : public NesMapper {
	Q_OBJECT
public:
	void reset();
	void writeHigh(quint16 address, quint8 data);
};

#endif // MAPPER013_H
