#ifndef MAPPER6_H
#define MAPPER6_H

#include "nesmapper.h"
#include "nescpumemorymapper.h"
#include "nesppumemorymapper.h"

class NesMapper6Data;
class PpuMapper6;

class CpuMapper6 : public NesCpuMemoryMapper {
	Q_OBJECT
public:
	explicit CpuMapper6(NesMapper *mapper);
	~CpuMapper6();
	void reset();
	void write(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);

	void save(QDataStream &s);
	bool load(QDataStream &s);
private:
	NesMapper6Data *d;
	PpuMapper6 *m_ppuMapper;
	friend class PpuMapper6;
};

class PpuMapper6: public NesPpuMemoryMapper {
	Q_OBJECT
public:
	explicit PpuMapper6(NesMapper *mapper);
	void reset();
	void horizontalSync(int scanline);
private:
	NesMapper6Data *d;
};

class NesMapper6Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER6_H
