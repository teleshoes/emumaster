#ifndef MAPPER229_H
#define MAPPER229_H

#include "nesmapper.h"
#include "nescpumemorymapper.h"
#include "nesppumemorymapper.h"

class CpuMapper229 : public NesCpuMemoryMapper {
	Q_OBJECT
public:
	explicit CpuMapper229(NesMapper *mapper);
	void writeHigh(quint16 address, quint8 data);
};

#define PpuMapper229 NesPpuMemoryMapper

class NesMapper229Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER229_H
