#ifndef MAPPER32_H
#define MAPPER32_H

#include "nesmapper.h"
#include "nescpumemorymapper.h"
#include "nesppumemorymapper.h"

class PpuMapper32;

class CpuMapper32 : public NesCpuMemoryMapper {
	Q_OBJECT
public:
	explicit CpuMapper32(NesMapper *mapper);
	void reset();
	void writeHigh(quint16 address, quint8 data);

	void save(QDataStream &s);
	bool load(QDataStream &s);
private:
	quint8 m_patch;
	quint8 m_reg;
};

#define PpuMapper32 NesPpuMemoryMapper

class NesMapper32Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER32_H
