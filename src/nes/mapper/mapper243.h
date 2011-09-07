#ifndef MAPPER243_H
#define MAPPER243_H

#include "../nesmapper.h"

class Mapper243 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeLow(quint16 address, quint8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	quint8 reg[4];
};

#endif // MAPPER243_H
