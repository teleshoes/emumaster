#ifndef MAPPER225_H
#define MAPPER225_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper225 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper225(NesMapper *mapper);
	void reset();

	void writeHigh(quint16 address, quint8 data);
private:
	NesPpuMapper *ppuMapper;
};

#define PpuMapper225 NesPpuMapper

class NesMapper225Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER225_H
