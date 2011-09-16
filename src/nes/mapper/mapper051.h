#ifndef MAPPER051_H
#define MAPPER051_H

#include "../mapper.h"

class Mapper051 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeLow(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);

	void updateBanks();

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	quint32 mode;
	quint32 bank;
};

#endif // MAPPER051_H
