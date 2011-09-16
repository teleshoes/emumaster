#ifndef MAPPER069_H
#define MAPPER069_H

#include "../mapper.h"

class Mapper069 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeHigh(quint16 address, quint8 data);
	void clock(uint cycles);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	quint8 patch;
	quint8 reg;
	quint8 irq_enable;
	quint32 irq_counter;
};

#endif // MAPPER069_H
