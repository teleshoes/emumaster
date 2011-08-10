#ifndef MAPPER233_H
#define MAPPER233_H

#include "nesmapper.h"
#include "nescpumemorymapper.h"
#include "nesppumemorymapper.h"

class CpuMapper233 : public NesCpuMemoryMapper {
	Q_OBJECT
public:
	explicit CpuMapper233(NesMapper *mapper);
	void writeHigh(quint16 address, quint8 data);
};

#define PpuMapper233 NesPpuMemoryMapper

class NesMapper233Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER233_H
