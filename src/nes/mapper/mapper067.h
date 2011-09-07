#ifndef MAPPER067_H
#define MAPPER067_H

#include "../nesmapper.h"

class Mapper067 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeHigh(quint16 address, quint8 data);
	void clock(uint cycles);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	quint8 irq_enable;
	quint8 irq_toggle;
	quint32 irq_counter;
};

#endif // MAPPER067_H
