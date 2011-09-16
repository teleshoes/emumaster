#ifndef MAPPER255_H
#define MAPPER255_H

#include "../mapper.h"

class Mapper255 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	quint8 readLow(quint16 address);
	void writeLow(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	quint8 reg[4];
};

#endif // MAPPER255_H
