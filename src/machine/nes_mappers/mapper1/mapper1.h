#ifndef MAPPER1_H
#define MAPPER1_H

#include "nesmapper.h"
#include "nescpumemorymapper.h"
#include "nesppumemorymapper.h"

class CpuMapper1 : public NesCpuMemoryMapper {
	Q_OBJECT
public:
	explicit CpuMapper1(NesMapper *mapper);
	void writeHigh(quint16 address, quint8 data);
private:
	NesPpuMemoryMapper::Mirroring mirroringFromRegs() const;

	quint8 patch;
	quint8 wram_patch;
	quint16 last_addr;
	quint8 reg[4];
	quint8 shift, regbuf;
};

#define PpuMapper1 NesPpuMemoryMapper

class NesMapper1Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER1_H
