#ifndef MAPPER69_H
#define MAPPER69_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class PpuMapper69;

class CpuMapper69 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper69(NesMapper *mapper);
	void reset();

	void writeHigh(quint16 address, quint8 data);
	void clock(uint cycles);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	NesPpuMapper *ppuMapper;

	quint8 patch;
	quint8 reg;
	quint8 irq_enable;
	quint32 irq_counter;

	friend class PpuMapper69;
};

#define PpuMapper69 NesPpuMapper

class NesMapper69Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER69_H
