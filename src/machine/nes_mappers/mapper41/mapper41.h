#ifndef MAPPER41_H
#define MAPPER41_H

#include "nesmapper.h"
#include "nescpumemorymapper.h"
#include "nesppumemorymapper.h"

class PpuMapper41;

class CpuMapper41 : public NesCpuMemoryMapper {
	Q_OBJECT
public:
	explicit CpuMapper41(NesMapper *mapper);
	void reset();
	void write(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);

	void save(QDataStream &s);
	bool load(QDataStream &s);
private:
	quint8 reg[2];
};

#define PpuMapper41 NesPpuMemoryMapper

class NesMapper41Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER41_H
