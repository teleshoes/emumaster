#ifndef MAPPER230_H
#define MAPPER230_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper230 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper230(NesMapper *mapper);
	void reset();

	void writeHigh(quint16 address, quint8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	NesPpuMapper *ppuMapper;

	quint8 rom_sw;
};

#define PpuMapper230 NesPpuMapper

class NesMapper230Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER230_H
