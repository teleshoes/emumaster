#ifndef MAPPER044_H
#define MAPPER044_H

#include "../nesmapper.h"

class Mapper044 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeLow(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);

	void horizontalSync(int scanline);

	void setBankCpu();
	void setBankPpu();

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	quint8 reg[8];
	quint8 patch;
	quint8 bank;
	quint8 prg0, prg1;
	quint8 chr01,chr23,chr4,chr5,chr6,chr7;
	quint8 irq_enable;
	quint8 irq_counter;
	quint8 irq_latch;
};

#endif // MAPPER044_H
