#ifndef MAPPER7_H
#define MAPPER7_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper7 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper7(NesMapper *mapper);
	void reset();
	void writeHigh(quint16 address, quint8 data);
private:
	quint8 patch;
};

#define PpuMapper7 NesPpuMapper

class NesMapper7Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER7_H
