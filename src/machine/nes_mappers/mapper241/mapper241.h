#ifndef MAPPER241_H
#define MAPPER241_H

#include "nesmapper.h"
#include "nescpumemorymapper.h"
#include "nesppumemorymapper.h"

class CpuMapper241 : public NesCpuMemoryMapper {
	Q_OBJECT
public:
	explicit CpuMapper241(NesMapper *mapper);
	void writeHigh(quint16 address, quint8 data);
};

#define PpuMapper241 NesPpuMemoryMapper

class NesMapper241Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER241_H
