#ifndef MAPPER34_H
#define MAPPER34_H

#include "nesmapper.h"
#include "nescpumemorymapper.h"
#include "nesppumemorymapper.h"

class PpuMapper34;

class CpuMapper34 : public NesCpuMemoryMapper {
	Q_OBJECT
public:
	explicit CpuMapper34(NesMapper *mapper);
	void reset();
	void write(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);
};

#define PpuMapper34 NesPpuMemoryMapper

class NesMapper34Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER34_H
