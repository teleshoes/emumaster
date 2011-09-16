#ifndef MAPPER227_H
#define MAPPER227_H

#include "../mapper.h"

class Mapper227 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeHigh(quint16 address, quint8 data);
};

#endif // MAPPER227_H
