#ifndef MAPPER202_H
#define MAPPER202_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper202 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper202(NesMapper *mapper);
	void reset();

	void writeEx(quint16 address, quint8 data);
	void writeLow(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);
private:
	void writeSub(quint16 address, quint8 data);

	NesPpuMapper *ppuMapper;
};

#define PpuMapper202 NesPpuMapper

class NesMapper202Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER202_H
