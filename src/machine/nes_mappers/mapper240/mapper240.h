#ifndef MAPPER240_H
#define MAPPER240_H

#include "nesmapper.h"
#include "nescpumemorymapper.h"
#include "nesppumemorymapper.h"

class CpuMapper240 : public NesCpuMemoryMapper {
	Q_OBJECT
public:
	explicit CpuMapper240(NesMapper *mapper);
	void write(quint16 address, quint8 data);
};

#define PpuMapper240 NesPpuMemoryMapper

class NesMapper240Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER240_H
