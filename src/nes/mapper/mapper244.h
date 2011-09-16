#ifndef MAPPER244_H
#define MAPPER244_H

#include "../mapper.h"

class Mapper244 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeHigh(quint16 address, quint8 data);
};

#endif // MAPPER244_H
