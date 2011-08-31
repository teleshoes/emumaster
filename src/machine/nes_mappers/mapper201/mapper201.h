#ifndef MAPPER201_H
#define MAPPER201_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper201 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper201(NesMapper *mapper);
	void reset();

	void writeHigh(quint16 address, quint8 data);
private:
	NesPpuMapper *ppuMapper;
};

#define PpuMapper201 NesPpuMapper

class NesMapper201Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER201_H
