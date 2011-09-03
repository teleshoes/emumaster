#ifndef MAPPER60_H
#define MAPPER60_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper60 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper60(NesMapper *mapper);
	void reset();

	void writeHigh(quint16 address, quint8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	NesPpuMapper *ppuMapper;

	quint8 patch;
	quint8 game_sel;
};

#define PpuMapper60 NesPpuMapper

class NesMapper60Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER60_H
