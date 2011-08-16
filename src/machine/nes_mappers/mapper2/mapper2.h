#ifndef MAPPER2_H
#define MAPPER2_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper2 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper2(NesMapper *mapper);
	void reset();
	void write(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);
private:
	quint8 patch;
	bool hasBattery;
};

#define PpuMapper2 NesPpuMapper

class NesMapper2Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER2_H
