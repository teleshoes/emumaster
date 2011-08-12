#ifndef MAPPER3_H
#define MAPPER3_H

#include "nesmapper.h"
#include "nescpumemorymapper.h"
#include "nesppumemorymapper.h"

class CpuMapper3 : public NesCpuMemoryMapper {
	Q_OBJECT
public:
	explicit CpuMapper3(NesMapper *mapper);
	void reset();
	void writeHigh(quint16 address, quint8 data);
};

#define PpuMapper3 NesPpuMemoryMapper

class NesMapper3Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER3_H
