#ifndef MAPPER2_H
#define MAPPER2_H

#include "nesmapper.h"
#include "nescpumemorymapper.h"
#include "nesppumemorymapper.h"

class CpuMapper2 : public NesCpuMemoryMapper {
	Q_OBJECT
public:
	explicit CpuMapper2(NesMapper *mapper);
	void writeHigh(quint16 address, quint8 data);
};

#define PpuMapper2 NesPpuMemoryMapper

class NesMapper2Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};


#endif // MAPPER2_H
