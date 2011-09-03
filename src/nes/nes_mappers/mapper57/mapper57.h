#ifndef MAPPER57_H
#define MAPPER57_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper57 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper57(NesMapper *mapper);
	void reset();

	void writeHigh(quint16 address, quint8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	NesPpuMapper *ppuMapper;

	quint8 reg;
};

#define PpuMapper57 NesPpuMapper

class NesMapper57Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER57_H
