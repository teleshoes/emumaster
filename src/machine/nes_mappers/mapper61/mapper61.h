#ifndef MAPPER61_H
#define MAPPER61_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper61 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper61(NesMapper *mapper);
	void reset();

	void writeHigh(quint16 address, quint8 data);
};

#define PpuMapper61 NesPpuMapper

class NesMapper61Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER61_H
