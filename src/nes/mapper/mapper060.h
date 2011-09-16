#ifndef MAPPER060_H
#define MAPPER060_H

#include "../mapper.h"

class Mapper060 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeHigh(quint16 address, quint8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	quint8 patch;
	quint8 game_sel;
};

#endif // MAPPER060_H
