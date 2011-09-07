#ifndef MAPPER229_H
#define MAPPER229_H

#include "../nesmapper.h"

class Mapper229 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeHigh(quint16 address, quint8 data);
};

#endif // MAPPER229_H
