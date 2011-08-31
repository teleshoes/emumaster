#ifndef MAPPER246_H
#define MAPPER246_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper246 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper246(NesMapper *mapper);
	void reset();

	void writeHigh(quint16 address, quint8 data);
private:
	NesPpuMapper *ppuMapper;
};

#define PpuMapper246 NesPpuMapper

class NesMapper246Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER246_H
