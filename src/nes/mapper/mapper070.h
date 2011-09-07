#ifndef MAPPER070_H
#define MAPPER070_H

#include "../nesmapper.h"

class Mapper070 : public NesMapper {
	Q_OBJECT
public:
	void reset();
	void writeHigh(quint16 address, quint8 data);
private:
	quint8 patch;
};

#endif // MAPPER070_H
