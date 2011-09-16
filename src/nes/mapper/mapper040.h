#ifndef MAPPER040_H
#define MAPPER040_H

#include "../mapper.h"

class Mapper040 : public NesMapper {
	Q_OBJECT
public:
	void reset();
	void writeHigh(quint16 address, quint8 data);

	void horizontalSync(int scanline);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	quint8 irq_enable;
	int irq_line;
};

#endif // MAPPER040_H
