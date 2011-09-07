#ifndef MAPPER226_H
#define MAPPER226_H

#include "../nesmapper.h"

class Mapper226 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeHigh(quint16 address, quint8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	quint8 reg[2];
};

#endif // MAPPER226_H
