#ifndef MAPPER065_H
#define MAPPER065_H

#include "../nesmapper.h"

class Mapper065 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeHigh(quint16 address, quint8 data);
	void clock(uint cycles);

	void horizontalSync(int scanline);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	quint8 patch;
	quint8 irq_enable;
	quint32 irq_counter;
	quint32 irq_latch;
};

#endif // MAPPER065_H
