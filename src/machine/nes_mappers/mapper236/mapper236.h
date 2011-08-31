#ifndef MAPPER236_H
#define MAPPER236_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper236 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper236(NesMapper *mapper);
	void reset();

	void writeHigh(quint16 address, quint8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	NesPpuMapper *ppuMapper;

	quint8 bank;
	quint8 mode;
};

#define PpuMapper236 NesPpuMapper

class NesMapper236Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER236_H
