#ifndef MAPPER0_H
#define MAPPER0_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper0 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper0(NesMapper *mapper);
	void reset();
};

#define PpuMapper0 NesPpuMapper

class NesMapper0Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER0_H
