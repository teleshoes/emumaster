#ifndef MAPPER057_H
#define MAPPER057_H

#include "../mapper.h"

class Mapper057 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeHigh(quint16 address, quint8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	quint8 reg;
};

#endif // MAPPER057_H
