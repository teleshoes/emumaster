#ifndef MAPPER202_H
#define MAPPER202_H

#include "../nesmapper.h"

class Mapper202 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeEx(quint16 address, quint8 data);
	void writeLow(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);
private:
	void writeSub(quint16 address, quint8 data);
};

#endif // MAPPER202_H
