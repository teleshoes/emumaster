#ifndef MAPPER243_H
#define MAPPER243_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper243 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper243(NesMapper *mapper);
	void reset();

	void writeLow(quint16 address, quint8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	NesPpuMapper *ppuMapper;

	quint8 reg[4];
};

#define PpuMapper243 NesPpuMapper

class NesMapper243Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER243_H
