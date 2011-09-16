#ifndef MAPPER045_H
#define MAPPER045_H

#include "../mapper.h"

class Mapper045 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeLow(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);

	void horizontalSync(int scanline);

	void setBankCpu(uint page, uint bank);
	void setBankPpu();

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	quint8 patch;
	quint8 reg[8];
	quint8 prg0, prg1, prg2, prg3;
	quint8 chr0, chr1, chr2, chr3, chr4, chr5, chr6, chr7;
	quint8 p[4];
	qint32 c[8];
	quint8 irq_enable;
	quint8 irq_counter;
	quint8 irq_latch;
	quint8 irq_latched;
	quint8 irq_reset;
};

#endif // MAPPER045_H
