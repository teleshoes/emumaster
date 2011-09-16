#ifndef MAPPER11_H
#define MAPPER11_H

#include "../mapper.h"

class Mapper011 : public NesMapper {
	Q_OBJECT
public:
	void reset();
	void writeHigh(quint16 address, quint8 data);
};

#endif // MAPPER11_H
