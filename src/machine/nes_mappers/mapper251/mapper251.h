#ifndef MAPPER251_H
#define MAPPER251_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper251 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper251(NesMapper *mapper);
	void reset();

	void writeLow(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);

	void setBank();

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	NesPpuMapper *ppuMapper;

	quint8 reg[11];
	quint8 breg[4];
};

#define PpuMapper251 NesPpuMapper

class NesMapper251Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER251_H
