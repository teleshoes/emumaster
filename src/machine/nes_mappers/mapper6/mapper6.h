#ifndef MAPPER6_H
#define MAPPER6_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class NesMapper6Data;
class PpuMapper6;

class CpuMapper6 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper6(NesMapper *mapper);
	~CpuMapper6();
	void reset();
	void writeLow(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	NesMapper6Data *d;
	PpuMapper6 *ppuMapper;
	friend class PpuMapper6;
};

class PpuMapper6: public NesPpuMapper {
	Q_OBJECT
public:
	explicit PpuMapper6(NesMapper *mapper);
	void reset();

	void horizontalSync(int scanline);
private:
	NesMapper6Data *d;
	CpuMapper6 *cpuMapper;
};

class NesMapper6Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER6_H
