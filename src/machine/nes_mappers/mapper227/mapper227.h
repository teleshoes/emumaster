#ifndef MAPPER227_H
#define MAPPER227_H

#include "nesmapper.h"
#include "nescpumemorymapper.h"
#include "nesppumemorymapper.h"

class CpuMapper227 : public NesCpuMemoryMapper {
	Q_OBJECT
public:
	explicit CpuMapper227(NesMapper *mapper);
	void writeHigh(quint16 address, quint8 data);
};

#define PpuMapper227 NesPpuMemoryMapper

class NesMapper227Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER227_H
