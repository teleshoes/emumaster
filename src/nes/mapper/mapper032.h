#ifndef MAPPER32_H
#define MAPPER32_H

#include "../mapper.h"

class Mapper032 : public NesMapper {
	Q_OBJECT
public:
	void reset();
	void writeHigh(quint16 address, quint8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	quint8 patch;
	quint8 reg;
};

#endif // MAPPER32_H
