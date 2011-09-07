#ifndef MAPPER015_H
#define MAPPER015_H

#include "../nesmapper.h"

class Mapper015 : public NesMapper {
	Q_OBJECT
public:
	void reset();
	void writeHigh(quint16 address, quint8 data);
};

#endif // MAPPER015_H
