#ifndef MAPPER58_H
#define MAPPER58_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class PpuMapper58;

class CpuMapper58 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper58(NesMapper *mapper);
	void reset();
	void writeHigh(quint16 address, quint8 data);
};

#define PpuMapper58 NesPpuMapper

class NesMapper58Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER58_H
