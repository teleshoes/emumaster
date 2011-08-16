#ifndef MAPPER244_H
#define MAPPER244_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper244 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper244(NesMapper *mapper);
	void writeHigh(quint16 address, quint8 data);
};

#define PpuMapper244 NesPpuMapper

class NesMapper244Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER244_H
