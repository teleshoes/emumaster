#ifndef MAPPER251_H
#define MAPPER251_H

#include "../nesmapper.h"

class Mapper251 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeLow(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);

	void setBank();

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	quint8 reg[11];
	quint8 breg[4];
};

#endif // MAPPER251_H
