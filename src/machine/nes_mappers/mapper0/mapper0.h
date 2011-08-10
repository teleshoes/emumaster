#ifndef MAPPER0_H
#define MAPPER0_H

#include "nesmapper.h"
#include "nescpumemorymapper.h"
#include "nesppumemorymapper.h"

#define CpuMapper0 NesCpuMemoryMapper
#define PpuMapper0 NesPpuMemoryMapper

class NesMapper0Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER0_H
