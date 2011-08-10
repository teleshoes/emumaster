#ifndef MAPPER231_H
#define MAPPER231_H

#include "nesmapper.h"
#include "nescpumemorymapper.h"
#include "nesppumemorymapper.h"

class CpuMapper231 : public NesCpuMemoryMapper {
	Q_OBJECT
public:
	explicit CpuMapper231(NesMapper *mapper);
	void writeHigh(quint16 address, quint8 data);
};

#define PpuMapper231 NesPpuMemoryMapper

class NesMapper231Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER231_H
