#ifndef MAPPER071_H
#define MAPPER071_H

#include "../mapper.h"

class Mapper071 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeLow(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);
};

#endif // MAPPER071_H
