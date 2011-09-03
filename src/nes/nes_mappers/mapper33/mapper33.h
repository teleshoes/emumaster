#ifndef MAPPER33_H
#define MAPPER33_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper33 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper33(NesMapper *mapper);
	void reset();

	void writeHigh(quint16 address, quint8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);

	void updateBanks();
private:
	NesPpuMapper *ppuMapper;

	quint8 patch;

	quint8 reg[7];

	quint8 irq_enable;
	quint8 irq_counter;
	quint8 irq_latch;

	friend class PpuMapper33;
};

class PpuMapper33 : public NesPpuMapper {
	Q_OBJECT
public:
	explicit PpuMapper33(NesMapper *mapper);
	void reset();

	void horizontalSync(int scanline);
private:
	CpuMapper33 *cpuMapper;
};

class NesMapper33Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER33_H
