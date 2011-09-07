#ifndef MAPPER027_H
#define MAPPER027_H

#include "../nesmapper.h"

class Mapper027 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeHigh(quint16 address, quint8 data);

	void horizontalSync(int scanline);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	quint8 reg[9];

	quint8 irq_enable;
	quint8 irq_counter;
	quint8 irq_latch;
	int irq_clock;

	friend class PpuMapper27;
};

#endif // MAPPER027_H
