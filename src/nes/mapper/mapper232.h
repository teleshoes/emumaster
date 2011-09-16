#ifndef MAPPER232_H
#define MAPPER232_H

#include "../mapper.h"

class Mapper232 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeLow(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	quint8 reg[2];
};

#endif // MAPPER232_H
