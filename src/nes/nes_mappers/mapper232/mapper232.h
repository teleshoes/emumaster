#ifndef MAPPER232_H
#define MAPPER232_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper232 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper232(NesMapper *mapper);
	void reset();

	void writeLow(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	NesPpuMapper *ppuMapper;

	quint8 reg[2];
};

#define PpuMapper232 NesPpuMapper

class NesMapper232Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER232_H
