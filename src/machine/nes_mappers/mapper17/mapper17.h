#ifndef MAPPER17_H
#define MAPPER17_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class NesMapper17Data;
class PpuMapper17;

class CpuMapper17 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper17(NesMapper *mapper);
	~CpuMapper17();
	void reset();
	void writeLow(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	NesMapper17Data *d;
	PpuMapper17 *ppuMapper;
	friend class PpuMapper17;
};

class PpuMapper17: public NesPpuMapper {
	Q_OBJECT
public:
	explicit PpuMapper17(NesMapper *mapper);
	void reset();

	void horizontalSync(int scanline);
private:
	NesMapper17Data *d;
	CpuMapper17 *cpuMapper;
};

class NesMapper17Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER17_H
