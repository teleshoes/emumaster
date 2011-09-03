#ifndef MAPPER45_H
#define MAPPER45_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class PpuMapper45;

class CpuMapper45 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper45(NesMapper *mapper);
	void reset();

	void writeLow(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);

	void setBankCpu(uint page, uint bank);
	void setBankPpu();

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	PpuMapper45 *ppuMapper;

	quint8 patch;
	quint8 reg[8];
	quint8 prg0, prg1, prg2, prg3;
	quint8 chr0, chr1, chr2, chr3, chr4, chr5, chr6, chr7;
	quint8 p[4];
	qint32 c[8];
	quint8 irq_enable;
	quint8 irq_counter;
	quint8 irq_latch;
	quint8 irq_latched;
	quint8 irq_reset;

	friend class PpuMapper45;
};

class PpuMapper45: public NesPpuMapper {
	Q_OBJECT
public:
	explicit PpuMapper45(NesMapper *mapper);
	void reset();

	void horizontalSync(int scanline);
private:
	CpuMapper45 *cpuMapper;
};

class NesMapper45Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER45_H
