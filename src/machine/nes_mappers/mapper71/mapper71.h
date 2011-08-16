#ifndef MAPPER71_H
#define MAPPER71_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper71 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper71(NesMapper *mapper);
	void writeHigh(quint16 address, quint8 data);
	void write(quint16 address, quint8 data);
};

#define PpuMapper71 NesPpuMapper

class NesMapper71Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER71_H
