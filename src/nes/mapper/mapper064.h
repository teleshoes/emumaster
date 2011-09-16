#ifndef MAPPER064_H
#define MAPPER064_H

#include "../mapper.h"

class Mapper064 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeHigh(quint16 address, quint8 data);
	void clock(uint cycles);

	void horizontalSync(int scanline);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	quint8 reg[3];
	quint8 irq_enable;
	quint8 irq_mode;
	qint32 irq_counter;
	qint32 irq_counter2;
	quint8 irq_latch;
	quint8 irq_reset;
};

#endif // MAPPER064_H
