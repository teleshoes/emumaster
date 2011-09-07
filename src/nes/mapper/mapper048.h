#ifndef MAPPER048_H
#define MAPPER048_H

#include "../nesmapper.h"

class Mapper048 : public NesMapper {
	Q_OBJECT
public:
	void reset();
	void writeHigh(quint16 address, quint8 data);

	void horizontalSync(int scanline);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	quint8 reg;
	quint8 irq_enable;
	quint8 irq_counter;
};

#endif // MAPPER048_H
