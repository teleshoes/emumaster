#ifndef MAPPER34_H
#define MAPPER34_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class PpuMapper34;

class CpuMapper34 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper34(NesMapper *mapper);
	void reset();
	void writeLow(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);
private:
	NesPpuMapper *ppuMapper;
};

#define PpuMapper34 NesPpuMapper

class NesMapper34Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER34_H
