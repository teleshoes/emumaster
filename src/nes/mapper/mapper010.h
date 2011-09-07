#ifndef MAPPER010_H
#define MAPPER010_H

#include "../nesmapper.h"

class Mapper010 : public NesMapper {
	Q_OBJECT
public:
	void reset();
	void writeHigh(quint16 address, quint8 data);

	void characterLatch(quint16 address);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	quint8 reg[4];
	quint8 latch_a, latch_b;
};

#endif // MAPPER010_H
