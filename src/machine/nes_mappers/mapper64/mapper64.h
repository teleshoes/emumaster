#ifndef MAPPER64_H
#define MAPPER64_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class PpuMapper64;

class CpuMapper64 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper64(NesMapper *mapper);
	void reset();

	void writeHigh(quint16 address, quint8 data);
	void clock(uint cycles);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	PpuMapper64 *ppuMapper;

	quint8 reg[3];
	quint8 irq_enable;
	quint8 irq_mode;
	quint32 irq_counter;
	quint32 irq_counter2;
	quint8 irq_latch;
	quint8 irq_reset;

	friend class PpuMapper64;
};

class PpuMapper64: public NesPpuMapper {
	Q_OBJECT
public:
	explicit PpuMapper64(NesMapper *mapper);
	void reset();

	void horizontalSync(int scanline);
private:
	CpuMapper64 *cpuMapper;
};

class NesMapper64Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER64_H
