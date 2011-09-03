#ifndef MAPPER50_H
#define MAPPER50_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class PpuMapper50;

class CpuMapper50 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper50(NesMapper *mapper);
	void reset();

	void writeEx(quint16 address, quint8 data);
	void writeLow(quint16 address, quint8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	PpuMapper50 *ppuMapper;

	quint8 irq_enable;

	friend class PpuMapper50;
};

class PpuMapper50: public NesPpuMapper {
	Q_OBJECT
public:
	explicit PpuMapper50(NesMapper *mapper);
	void reset();

	void horizontalSync(int scanline);
private:
	CpuMapper50 *cpuMapper;
};

class NesMapper50Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER50_H
