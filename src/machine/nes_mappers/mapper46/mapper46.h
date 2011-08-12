#ifndef MAPPER46_H
#define MAPPER46_H

#include "nesmapper.h"
#include "nescpumemorymapper.h"
#include "nesppumemorymapper.h"

class PpuMapper46;

class CpuMapper46 : public NesCpuMemoryMapper {
	Q_OBJECT
public:
	explicit CpuMapper46(NesMapper *mapper);
	void reset();
	void write(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);

	void save(QDataStream &s);
	bool load(QDataStream &s);
private:
	void updateBanks();

	quint32 m_reg[4];
};

#define PpuMapper46 NesPpuMemoryMapper

class NesMapper46Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER46_H
