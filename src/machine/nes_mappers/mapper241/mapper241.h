#ifndef MAPPER241_H
#define MAPPER241_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper241 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper241(NesMapper *mapper);
	void writeHigh(quint16 address, quint8 data);
};

#define PpuMapper241 NesPpuMapper

class NesMapper241Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER241_H
