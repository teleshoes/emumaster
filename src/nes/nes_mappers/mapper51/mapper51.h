#ifndef MAPPER51_H
#define MAPPER51_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class PpuMapper51;

class CpuMapper51 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper51(NesMapper *mapper);
	void reset();

	void writeLow(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);

	void updateBanks();

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	NesPpuMapper *ppuMapper;

	quint32 mode;
	quint32 bank;
};

#define PpuMapper51 NesPpuMapper

class NesMapper51Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER51_H
