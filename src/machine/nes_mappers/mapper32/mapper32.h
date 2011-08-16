#ifndef MAPPER32_H
#define MAPPER32_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class PpuMapper32;

class CpuMapper32 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper32(NesMapper *mapper);
	void reset();
	void writeHigh(quint16 address, quint8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	NesPpuMapper *ppuMapper;

	quint8 patch;
	quint8 reg;
};

#define PpuMapper32 NesPpuMapper

class NesMapper32Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER32_H
