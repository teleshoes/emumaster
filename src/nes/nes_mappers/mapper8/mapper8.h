#ifndef MAPPER8_H
#define MAPPER8_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper8 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper8(NesMapper *mapper);
	void reset();
	void writeHigh(quint16 address, quint8 data);
};

#define PpuMapper8 NesPpuMapper

class NesMapper8Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER8_H
