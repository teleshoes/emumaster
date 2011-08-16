#ifndef MAPPER27_H
#define MAPPER27_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper27 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper27(NesMapper *mapper);
	void reset();

	void writeHigh(quint16 address, quint8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	NesPpuMapper *ppuMapper;

	quint8 reg[9];

	quint8 irq_enable;
	quint8 irq_counter;
	quint8 irq_latch;
	int irq_clock;

	friend class PpuMapper27;
};

class PpuMapper27 : public NesPpuMapper {
	Q_OBJECT
public:
	explicit PpuMapper27(NesMapper *mapper);
	void reset();

	void horizontalSync(int scanline);
private:
	CpuMapper27 *cpuMapper;
};

class NesMapper27Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER27_H
