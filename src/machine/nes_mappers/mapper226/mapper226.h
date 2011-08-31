#ifndef MAPPER226_H
#define MAPPER226_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper226 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper226(NesMapper *mapper);
	void reset();

	void writeHigh(quint16 address, quint8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	NesPpuMapper *ppuMapper;

	quint8 reg[2];
};

#define PpuMapper226 NesPpuMapper

class NesMapper226Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER226_H
