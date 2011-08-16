#ifndef MAPPER42_H
#define MAPPER42_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class PpuMapper42;

class CpuMapper42 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper42(NesMapper *mapper);
	void reset();
	void writeHigh(quint16 address, quint8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	PpuMapper42 *ppuMapper;

	quint8 irq_enable;
	quint8 irq_counter;

	friend class PpuMapper42;
};

class PpuMapper42: public NesPpuMapper {
	Q_OBJECT
public:
	explicit PpuMapper42(NesMapper *mapper);
	void reset();

	void horizontalSync(int scanline);
private:
	CpuMapper42 *cpuMapper;
};

class NesMapper42Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER42_H
