#ifndef MAPPER041_H
#define MAPPER041_H

#include "../nesmapper.h"

class Mapper041 : public NesMapper {
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

#endif // MAPPER041_H
