#ifndef MAPPER15_H
#define MAPPER15_H

#include "nesmapper.h"
#include "nescpumemorymapper.h"
#include "nesppumemorymapper.h"

class PpuMapper15;

class CpuMapper15 : public NesCpuMemoryMapper {
	Q_OBJECT
public:
	explicit CpuMapper15(NesMapper *mapper);
	void reset();
	void writeHigh(quint16 address, quint8 data);
};

#define PpuMapper15 NesPpuMemoryMapper

class NesMapper15Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER15_H
