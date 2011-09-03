#ifndef MAPPER68_H
#define MAPPER68_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper68 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper68(NesMapper *mapper);
	void reset();

	void writeHigh(quint16 address, quint8 data);

	void updateBanks();

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	NesPpuMapper *ppuMapper;

	quint8 reg[4];
};

#define PpuMapper68 NesPpuMapper

class NesMapper68Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER68_H
