#ifndef MAPPER7_H
#define MAPPER7_H

#include "nesmapper.h"
#include "nescpumemorymapper.h"
#include "nesppumemorymapper.h"

class PpuMapper7;

class CpuMapper7 : public NesCpuMemoryMapper {
	Q_OBJECT
public:
	explicit CpuMapper7(NesMapper *mapper);
	void reset();
	void writeHigh(quint16 address, quint8 data);
private:
	quint8 m_patch;
};

#define PpuMapper7 NesPpuMemoryMapper

class NesMapper7Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER7_H
