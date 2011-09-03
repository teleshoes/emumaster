#ifndef MAPPER1_H
#define MAPPER1_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class CpuMapper1 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper1(NesMapper *mapper);
	void reset();
	void writeHigh(quint16 address, quint8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	NesPpuMapper::Mirroring mirroringFromRegs() const;

	NesPpuMapper *ppuMapper;
	quint8 patch;
	quint8 wram_patch;
	quint8 wram_bank;
	quint8 wram_count;
	quint16 last_addr;
	quint8 reg[4];
	quint8 shift, regbuf;
};

#define PpuMapper1 NesPpuMapper

class NesMapper1Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER1_H
