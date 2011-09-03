#ifndef MAPPER18_H
#define MAPPER18_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper18 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper18(NesMapper *mapper);
	void reset();

	void writeHigh(quint16 address, quint8 data);

	void clock(uint cycles);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	NesPpuMapper *ppuMapper;

	quint8 reg[11];

	quint8 irq_enable;
	quint8 irq_mode;
	int irq_latch;
	int irq_counter;
};

#define PpuMapper18 NesPpuMapper

class NesMapper18Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER18_H
