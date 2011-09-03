#ifndef MAPPER255_H
#define MAPPER255_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper255 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper255(NesMapper *mapper);
	void reset();

	quint8 readLow(quint16 address);
	void writeLow(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	NesPpuMapper *ppuMapper;

	quint8 reg[4];
};

#define PpuMapper255 NesPpuMapper

class NesMapper255Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER255_H
