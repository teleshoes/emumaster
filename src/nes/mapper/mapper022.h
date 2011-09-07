#ifndef MAPPER022_H
#define MAPPER022_H

#include "../nesmapper.h"

class Mapper022 : public NesMapper {
	Q_OBJECT
public:
	void reset();
	void writeHigh(quint16 address, quint8 data);
};

#endif // MAPPER022_H
