#ifndef MAPPER23_H
#define MAPPER23_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper23 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper23(NesMapper *mapper);
	void reset();

	void writeHigh(quint16 address, quint8 data);
	void clock(uint cycles);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	NesPpuMapper *ppuMapper;

	quint16 addrmask;

	quint8 reg[9];

	quint8 irq_enable;
	quint8 irq_counter;
	quint8 irq_latch;
	int irq_clock;
};

#define PpuMapper23 NesPpuMapper

class NesMapper23Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER23_H
