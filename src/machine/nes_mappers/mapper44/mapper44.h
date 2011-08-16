#ifndef MAPPER44_H
#define MAPPER44_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class PpuMapper44;

class CpuMapper44 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper44(NesMapper *mapper);
	void reset();

	void writeLow(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);

	void setBankCpu();
	void setBankPpu();

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	PpuMapper44 *ppuMapper;

	quint8 reg[8];
	quint8 patch;
	quint8 bank;
	quint8 prg0, prg1;
	quint8 chr01,chr23,chr4,chr5,chr6,chr7;
	quint8 irq_enable;
	quint8 irq_counter;
	quint8 irq_latch;

	friend class PpuMapper44;
};

class PpuMapper44: public NesPpuMapper {
	Q_OBJECT
public:
	explicit PpuMapper44(NesMapper *mapper);
	void reset();

	void horizontalSync(int scanline);
private:
	CpuMapper44 *cpuMapper;
};

class NesMapper44Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER44_H
