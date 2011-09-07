#ifndef MAPPER018_H
#define MAPPER018_H

#include "../nesmapper.h"

class Mapper018 : public NesMapper {
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
	quint8 irq_mode;
	int irq_latch;
	int irq_counter;
};

#endif // MAPPER018_H
