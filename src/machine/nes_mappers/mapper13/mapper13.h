#ifndef MAPPER13_H
#define MAPPER13_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper13 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper13(NesMapper *mapper);
	void reset();
	void writeHigh(quint16 address, quint8 data);
};

#define PpuMapper13 NesPpuMapper

class NesMapper13Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER13_H
