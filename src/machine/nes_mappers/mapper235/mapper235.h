#ifndef MAPPER235_H
#define MAPPER235_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper235 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper235(NesMapper *mapper);
	void reset();

	void writeHigh(quint16 address, quint8 data);
private:
	NesPpuMapper *ppuMapper;
};

#define PpuMapper235 NesPpuMapper

class NesMapper235Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER235_H
