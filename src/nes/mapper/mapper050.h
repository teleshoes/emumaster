#ifndef MAPPER050_H
#define MAPPER050_H

#include "../nesmapper.h"

class Mapper050 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeEx(quint16 address, quint8 data);
	void writeLow(quint16 address, quint8 data);

	void horizontalSync(int scanline);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	quint8 irq_enable;
};

#endif // MAPPER050_H
