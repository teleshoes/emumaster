#ifndef MAPPER228_H
#define MAPPER228_H

#include "nesmapper.h"
#include "nescpumemorymapper.h"
#include "nesppumemorymapper.h"

class CpuMapper228 : public NesCpuMemoryMapper {
	Q_OBJECT
public:
	explicit CpuMapper228(NesMapper *mapper);
	void writeHigh(quint16 address, quint8 data);
};

#define PpuMapper228 NesPpuMemoryMapper

class NesMapper228Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER228_H
