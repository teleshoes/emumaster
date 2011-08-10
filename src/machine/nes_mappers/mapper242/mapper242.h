#ifndef MAPPER242_H
#define MAPPER242_H

#include "nesmapper.h"
#include "nescpumemorymapper.h"
#include "nesppumemorymapper.h"

class CpuMapper242 : public NesCpuMemoryMapper {
	Q_OBJECT
public:
	explicit CpuMapper242(NesMapper *mapper);
	void writeHigh(quint16 address, quint8 data);
};

#define PpuMapper242 NesPpuMemoryMapper

class NesMapper242Plugin : public NesMapperPlugin {
	Q_OBJECT
public: \
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER242_H
