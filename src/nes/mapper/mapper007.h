#ifndef MAPPER007_H
#define MAPPER007_H

#include "../mapper.h"

class Mapper007 : public NesMapper {
	Q_OBJECT
public:
	void reset();
	void writeHigh(quint16 address, quint8 data);
private:
	quint8 patch;
};

#endif // MAPPER007_H
