#ifndef MAPPER67_H
#define MAPPER67_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper67 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper67(NesMapper *mapper);
	void reset();

	void writeHigh(quint16 address, quint8 data);
	void clock(uint cycles);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	NesPpuMapper *ppuMapper;

	quint8 irq_enable;
	quint8 irq_toggle;
	quint32 irq_counter;
};

#define PpuMapper67 NesPpuMapper

class NesMapper67Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER67_H
