#ifndef MAPPER241_H
#define MAPPER241_H

#include "../nesmapper.h"

class Mapper241 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeHigh(quint16 address, quint8 data);
};

#endif // MAPPER241_H
