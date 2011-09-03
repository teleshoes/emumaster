#ifndef MAPPER47_H
#define MAPPER47_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class PpuMapper47;

class CpuMapper47 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper47(NesMapper *mapper);
	void reset();

	void writeLow(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);

	void setBankCpu();
	void setBankPpu();

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	PpuMapper47 *ppuMapper;

	quint8 reg[8];
	quint8 patch;
	quint8 bank;
	quint8 prg0, prg1;
	quint8 chr01,chr23,chr4,chr5,chr6,chr7;
	quint8 irq_enable;
	quint8 irq_counter;
	quint8 irq_latch;

	friend class PpuMapper47;
};

class PpuMapper47: public NesPpuMapper {
	Q_OBJECT
public:
	explicit PpuMapper47(NesMapper *mapper);
	void reset();

	void horizontalSync(int scanline);
private:
	CpuMapper47 *cpuMapper;
};

class NesMapper47Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER47_H
