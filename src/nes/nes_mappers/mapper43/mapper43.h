#ifndef MAPPER43_H
#define MAPPER43_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class PpuMapper43;

class CpuMapper43 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper43(NesMapper *mapper);
	void reset();

	quint8 readLow(quint16 address);
	void writeEx(quint16 address, quint8 data);
	void writeLow(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	PpuMapper43 *ppuMapper;

	quint8 irq_enable;
	int irq_counter;

	friend class PpuMapper43;
};

class PpuMapper43: public NesPpuMapper {
	Q_OBJECT
public:
	explicit PpuMapper43(NesMapper *mapper);
	void reset();

	void horizontalSync(int scanline);
private:
	CpuMapper43 *cpuMapper;
};

class NesMapper43Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER43_H
