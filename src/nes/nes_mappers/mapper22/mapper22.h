#ifndef MAPPER22_H
#define MAPPER22_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper22 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper22(NesMapper *mapper);
	void reset();
	void writeHigh(quint16 address, quint8 data);
private:
	NesPpuMapper *ppuMapper;
};

#define PpuMapper22 NesPpuMapper

class NesMapper22Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER22_H
