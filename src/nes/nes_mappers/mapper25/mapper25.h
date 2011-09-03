#ifndef MAPPER25_H
#define MAPPER25_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper25 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper25(NesMapper *mapper);
	void reset();

	void writeHigh(quint16 address, quint8 data);
	void clock(uint cycles);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	NesPpuMapper *ppuMapper;

	quint8 reg[11];

	quint8 irq_enable;
	quint8 irq_counter;
	quint8 irq_latch;
	quint8 irq_occur;
	int irq_clock;
};

#define PpuMapper25 NesPpuMapper

class NesMapper25Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER25_H
