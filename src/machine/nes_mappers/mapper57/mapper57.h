#ifndef MAPPER57_H
#define MAPPER57_H

#include "nesmapper.h"
#include "nescpumemorymapper.h"
#include "nesppumemorymapper.h"

class PpuMapper57;

class CpuMapper57 : public NesCpuMemoryMapper {
	Q_OBJECT
public:
	explicit CpuMapper57(NesMapper *mapper);
	void reset();
	void writeHigh(quint16 address, quint8 data);

	void save(QDataStream &s);
	bool load(QDataStream &s);
private:
	quint8 m_reg;
};

#define PpuMapper57 NesPpuMemoryMapper

class NesMapper57Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER57_H
