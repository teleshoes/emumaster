#ifndef MAPPER002_H
#define MAPPER002_H

#include "../nesmapper.h"

class Mapper002 : public NesMapper {
	Q_OBJECT
public:
	void reset();
	void writeLow(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);
private:
	quint8 patch;
	bool hasBattery;
};

#endif // MAPPER002_H
