#ifndef MAPPER40_H
#define MAPPER40_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class PpuMapper40;

class CpuMapper40 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper40(NesMapper *mapper);
	void reset();
	void writeHigh(quint16 address, quint8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	PpuMapper40 *ppuMapper;

	quint8 irq_enable;
	int irq_line;

	friend class PpuMapper40;
};

class PpuMapper40: public NesPpuMapper {
	Q_OBJECT
public:
	explicit PpuMapper40(NesMapper *mapper);
	void reset();

	void horizontalSync(int scanline);
private:
	CpuMapper40 *cpuMapper;
};

class NesMapper40Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER40_H
