#ifndef MAPPER228_H
#define MAPPER228_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper228 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper228(NesMapper *mapper);
	void reset();

	void writeHigh(quint16 address, quint8 data);
private:
	NesPpuMapper *ppuMapper;
};

#define PpuMapper228 NesPpuMapper

class NesMapper228Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER228_H
