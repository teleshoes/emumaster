#ifndef MAPPER242_H
#define MAPPER242_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper242 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper242(NesMapper *mapper);
	void reset();

	void writeHigh(quint16 address, quint8 data);
};

#define PpuMapper242 NesPpuMapper

class NesMapper242Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER242_H
