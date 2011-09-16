#ifndef MAPPER068_H
#define MAPPER068_H

#include "../mapper.h"

class Mapper068 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeHigh(quint16 address, quint8 data);

	void updateBanks();

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	quint8 reg[4];
};

#endif // MAPPER068_H
