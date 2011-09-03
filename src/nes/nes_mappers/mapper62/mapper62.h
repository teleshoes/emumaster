#ifndef MAPPER62_H
#define MAPPER62_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper62 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper62(NesMapper *mapper);
	void reset();

	void writeHigh(quint16 address, quint8 data);
private:
	NesPpuMapper *ppuMapper;
};

#define PpuMapper62 NesPpuMapper

class NesMapper62Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER62_H
