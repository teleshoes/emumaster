#ifndef MAPPER017_H
#define MAPPER017_H

#include "../mapper.h"

class Mapper017 : public NesMapper {
	Q_OBJECT
public:
	void reset();
	void writeLow(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);

	void horizontalSync(int scanline);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	quint8 irqEnable;
	int irqCounter;
	int irqLatch;
};

#endif // MAPPER017_H
