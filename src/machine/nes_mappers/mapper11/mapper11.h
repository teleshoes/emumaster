#ifndef MAPPER11_H
#define MAPPER11_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper11 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper11(NesMapper *mapper);
	void reset();
	void writeHigh(quint16 address, quint8 data);
};

#define PpuMapper11 NesPpuMapper

class NesMapper11Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER11_H
