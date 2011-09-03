#ifndef MAPPER70_H
#define MAPPER70_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper70 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper70(NesMapper *mapper);

	void reset();
	void writeHigh(quint16 address, quint8 data);
private:
	NesPpuMapper *ppuMapper;

	quint8 patch;
};

#define PpuMapper70 NesPpuMapper

class NesMapper70Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER70_H
