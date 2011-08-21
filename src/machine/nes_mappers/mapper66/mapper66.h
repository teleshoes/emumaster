#ifndef MAPPER66_H
#define MAPPER66_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper66 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper66(NesMapper *mapper);
	void reset();

	void writeLow(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);
private:
	NesPpuMapper *ppuMapper;
};

#define PpuMapper66 NesPpuMapper

class NesMapper66Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER66_H
