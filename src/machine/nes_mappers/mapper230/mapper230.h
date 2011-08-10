#ifndef MAPPER230_H
#define MAPPER230_H

#include "nesmapper.h"
#include "nescpumemorymapper.h"
#include "nesppumemorymapper.h"

class CpuMapper230 : public NesCpuMemoryMapper {
	Q_OBJECT
public:
	explicit CpuMapper230(NesMapper *mapper);
	void writeHigh(quint16 address, quint8 data);
private:
	bool m_switch;
};

#define PpuMapper230 NesPpuMemoryMapper

class NesMapper230Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER230_H
