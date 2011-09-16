#ifndef MAPPER025_H
#define MAPPER025_H

#include "../mapper.h"

class Mapper025 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeHigh(quint16 address, quint8 data);
	void clock(uint cycles);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	quint8 reg[11];

	quint8 irq_enable;
	quint8 irq_counter;
	quint8 irq_latch;
	quint8 irq_occur;
	int irq_clock;
};

#endif // MAPPER025_H
