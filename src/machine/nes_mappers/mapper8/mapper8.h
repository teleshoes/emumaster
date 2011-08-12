#ifndef MAPPER8_H
#define MAPPER8_H

#include "nesmapper.h"
#include "nescpumemorymapper.h"
#include "nesppumemorymapper.h"

class PpuMapper8;

class CpuMapper8 : public NesCpuMemoryMapper {
	Q_OBJECT
public:
	explicit CpuMapper8(NesMapper *mapper);
	void reset();
	void writeHigh(quint16 address, quint8 data);
};

#define PpuMapper8 NesPpuMemoryMapper

class NesMapper8Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER8_H
