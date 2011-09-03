#ifndef MAPPER200_H
#define MAPPER200_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper200 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper200(NesMapper *mapper);
	void reset();

	void writeHigh(quint16 address, quint8 data);
private:
	NesPpuMapper *ppuMapper;
};

#define PpuMapper200 NesPpuMapper

class NesMapper200Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER200_H
