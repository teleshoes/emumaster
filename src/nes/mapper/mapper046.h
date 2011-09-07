#ifndef MAPPER046_H
#define MAPPER046_H

#include "../nesmapper.h"

class Mapper046 : public NesMapper {
	Q_OBJECT
public:
	void reset();
	void writeLow(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	void updateBanks();

	quint32 reg[4];
};

#endif // MAPPER046_H
