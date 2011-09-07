#ifndef MAPPER230_H
#define MAPPER230_H

#include "../nesmapper.h"

class Mapper230 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeHigh(quint16 address, quint8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	quint8 rom_sw;
};

#endif // MAPPER230_H
