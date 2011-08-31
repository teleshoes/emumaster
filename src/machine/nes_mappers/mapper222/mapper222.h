#ifndef MAPPER222_H
#define MAPPER222_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper222 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper222(NesMapper *mapper);
	void reset();

	void writeHigh(quint16 address, quint8 data);
private:
	NesPpuMapper *ppuMapper;
};

#define PpuMapper222 NesPpuMapper

class NesMapper222Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER222_H
