#ifndef MAPPER023_H
#define MAPPER023_H

#include "../nesmapper.h"

class Mapper023 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeHigh(quint16 address, quint8 data);
	void clock(uint cycles);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	quint16 addrmask;

	quint8 reg[9];

	quint8 irq_enable;
	quint8 irq_counter;
	quint8 irq_latch;
	int irq_clock;
};

#endif // MAPPER023_H
