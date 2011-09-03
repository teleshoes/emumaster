#ifndef MAPPER3_H
#define MAPPER3_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper3 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper3(NesMapper *mapper);
	void reset();
	void writeHigh(quint16 address, quint8 data);
};

#define PpuMapper3 NesPpuMapper

class NesMapper3Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER3_H
