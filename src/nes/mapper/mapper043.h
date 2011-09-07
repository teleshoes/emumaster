#ifndef MAPPER043_H
#define MAPPER043_H

#include "../nesmapper.h"

class Mapper043 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	quint8 readLow(quint16 address);
	void writeEx(quint16 address, quint8 data);
	void writeLow(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);

	void horizontalSync(int scanline);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	quint8 irq_enable;
	int irq_counter;
};

#endif // MAPPER043_H
