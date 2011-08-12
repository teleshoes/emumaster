#ifndef MAPPER22_H
#define MAPPER22_H

#include "nesmapper.h"
#include "nescpumemorymapper.h"
#include "nesppumemorymapper.h"

class PpuMapper22;

class CpuMapper22 : public NesCpuMemoryMapper {
	Q_OBJECT
public:
	explicit CpuMapper22(NesMapper *mapper);
	void reset();
	void writeHigh(quint16 address, quint8 data);
private:
	NesPpuMemoryMapper *m_ppuMapper;
};

#define PpuMapper22 NesPpuMemoryMapper

class NesMapper22Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER22_H
