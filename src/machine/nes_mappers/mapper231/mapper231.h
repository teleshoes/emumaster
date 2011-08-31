#ifndef MAPPER231_H
#define MAPPER231_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper231 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper231(NesMapper *mapper);
	void reset();

	void writeHigh(quint16 address, quint8 data);
private:
	NesPpuMapper *ppuMapper;
};

#define PpuMapper231 NesPpuMapper

class NesMapper231Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER231_H
