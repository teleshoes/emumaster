#ifndef MAPPER236_H
#define MAPPER236_H

#include "../mapper.h"

class Mapper236 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeHigh(quint16 address, quint8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	quint8 bank;
	quint8 mode;
};

#endif // MAPPER236_H
