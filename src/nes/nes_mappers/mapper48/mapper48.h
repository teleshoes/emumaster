#ifndef MAPPER48_H
#define MAPPER48_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class PpuMapper48;

class CpuMapper48 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper48(NesMapper *mapper);
	void reset();
	void writeHigh(quint16 address, quint8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	PpuMapper48 *ppuMapper;

	quint8 reg;
	quint8 irq_enable;
	quint8 irq_counter;

	friend class PpuMapper48;
};

class PpuMapper48: public NesPpuMapper {
	Q_OBJECT
public:
	explicit PpuMapper48(NesMapper *mapper);
	void reset();

	void horizontalSync(int scanline);
private:
	CpuMapper48 *cpuMapper;
};

class NesMapper48Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER48_H
