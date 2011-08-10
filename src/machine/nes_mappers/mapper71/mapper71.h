#ifndef MAPPER71_H
#define MAPPER71_H

#include "nesmapper.h"
#include "nescpumemorymapper.h"
#include "nesppumemorymapper.h"

class CpuMapper71 : public NesCpuMemoryMapper {
	Q_OBJECT
public:
	explicit CpuMapper71(NesMapper *mapper);
	void writeHigh(quint16 address, quint8 data);
	void write(quint16 address, quint8 data);
};

#define PpuMapper71 NesPpuMemoryMapper

class NesMapper71Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER71_H
