#ifndef MAPPER227_H
#define MAPPER227_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper227 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper227(NesMapper *mapper);
	void reset();
	void writeHigh(quint16 address, quint8 data);
};

#define PpuMapper227 NesPpuMapper

class NesMapper227Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER227_H
