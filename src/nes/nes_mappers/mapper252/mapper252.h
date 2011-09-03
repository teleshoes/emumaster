#ifndef MAPPER252_H
#define MAPPER252_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper252 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper252(NesMapper *mapper);
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
	quint8 irq_occur;
	int irq_clock;
};

#define PpuMapper252 NesPpuMapper

class NesMapper252Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER252_H
