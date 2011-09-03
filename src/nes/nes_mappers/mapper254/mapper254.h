#ifndef MAPPER254_H
#define MAPPER254_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper254 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper254(NesMapper *mapper);
	void reset();
	quint8 readLow(quint16 address);
	void writeLow(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);

	void setBankCpu();
	void setBankPpu();

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	NesPpuMapper *ppuMapper;
	quint8 reg[8];
	quint8 prg0, prg1;
	quint8 chr01, chr23, chr4, chr5, chr6, chr7;

	quint8 irq_type;
	quint8 irq_enable;
	quint8 irq_counter;
	quint8 irq_latch;
	quint8 irq_request;
	quint8 protectflag;

	friend class PpuMapper254;
};

class PpuMapper254 : public NesPpuMapper {
	Q_OBJECT
public:
	explicit PpuMapper254(NesMapper *mapper);
	void reset();

	void horizontalSync(int scanline);
private:
	CpuMapper254 *cpuMapper;
};

class NesMapper254Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER254_H
