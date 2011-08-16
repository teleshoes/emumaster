#ifndef MAPPER21_H
#define MAPPER21_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper21 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper21(NesMapper *mapper);
	void reset();

	void writeHigh(quint16 address, quint8 data);
	void clock(uint cycles);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	NesPpuMapper *ppuMapper;

	quint8 reg[9];

	quint8 irq_enable;
	quint8 irq_counter;
	quint8 irq_latch;
	int irq_clock;
};

#define PpuMapper21 NesPpuMapper

class NesMapper21Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER21_H
