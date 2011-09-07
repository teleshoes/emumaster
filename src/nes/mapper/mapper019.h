#ifndef MAPPER019_H
#define MAPPER019_H

#include "../nesmapper.h"

class Mapper019 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	quint8 readLow(quint16 address);
	void writeLow(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);

	void clock(uint cycles);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	quint8 patch;
	quint8 exsound_enable;

	quint8 reg[3];
	quint8 exram[128];

	quint8 irq_enable;
	quint16 irq_counter;
};

#endif // MAPPER019_H
