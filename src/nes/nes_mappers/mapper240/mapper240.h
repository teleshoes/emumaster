#ifndef MAPPER240_H
#define MAPPER240_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper240 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper240(NesMapper *mapper);
	void reset();

	void writeLow(quint16 address, quint8 data);
private:
	NesPpuMapper *ppuMapper;
};

#define PpuMapper240 NesPpuMapper

class NesMapper240Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER240_H
