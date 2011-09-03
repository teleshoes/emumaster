#ifndef MAPPER19_H
#define MAPPER19_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper19 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper19(NesMapper *mapper);
	void reset();

	quint8 readLow(quint16 address);
	void writeLow(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);

	void clock(uint cycles);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	NesPpuMapper *ppuMapper;

	quint8 patch;
	quint8 exsound_enable;

	quint8 reg[3];
	quint8 exram[128];

	quint8 irq_enable;
	quint16 irq_counter;
};

#define PpuMapper19 NesPpuMapper

class NesMapper19Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER19_H
