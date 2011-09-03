#ifndef MAPPER24_H
#define MAPPER24_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper24 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper24(NesMapper *mapper);
	void reset();

	void writeHigh(quint16 address, quint8 data);
	void clock(uint cycles);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	NesPpuMapper *ppuMapper;

	quint8 irq_enable;
	quint8 irq_counter;
	quint8 irq_latch;
	int irq_clock;
};

#define PpuMapper24 NesPpuMapper

class NesMapper24Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER24_H
