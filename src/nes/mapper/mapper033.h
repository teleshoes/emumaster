#ifndef MAPPER033_H
#define MAPPER033_H

#include "../mapper.h"

class Mapper033 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeHigh(quint16 address, quint8 data);

	void horizontalSync(int scanline);

	bool save(QDataStream &s);
	bool load(QDataStream &s);

	void updateBanks();
private:
	quint8 patch;

	quint8 reg[7];

	quint8 irq_enable;
	quint8 irq_counter;
	quint8 irq_latch;
};

#endif // MAPPER033_H
