#ifndef MAPPER65_H
#define MAPPER65_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class PpuMapper65;

class CpuMapper65 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper65(NesMapper *mapper);
	void reset();

	void writeHigh(quint16 address, quint8 data);
	void clock(uint cycles);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	PpuMapper65 *ppuMapper;

	quint8 patch;
	quint8 irq_enable;
	quint32 irq_counter;
	quint32 irq_latch;

	friend class PpuMapper65;
};

class PpuMapper65: public NesPpuMapper {
	Q_OBJECT
public:
	explicit PpuMapper65(NesMapper *mapper);
	void reset();

	void horizontalSync(int scanline);
private:
	CpuMapper65 *cpuMapper;
};

class NesMapper65Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER65_H
