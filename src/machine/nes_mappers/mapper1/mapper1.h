#ifndef MAPPER1_H
#define MAPPER1_H

#include "nesmapper.h"
#include "nescpumemorymapper.h"
#include "nesppumemorymapper.h"

class CpuMapper1 : public NesCpuMemoryMapper {
	Q_OBJECT
public:
	explicit CpuMapper1(NesMapper *mapper);
	void reset();
	void writeHigh(quint16 address, quint8 data);

	void save(QDataStream &s);
	bool load(QDataStream &s);
private:
	NesPpuMemoryMapper::Mirroring mirroringFromRegs() const;

	NesPpuMemoryMapper *m_ppuMemory;
	quint8 m_patch;
	quint8 m_wramPatch;
	quint8 m_wramBank;
	quint8 m_wramCount;
	quint16 m_lastAddress;
	quint8 m_reg[4];
	quint8 m_shift, m_regbuf;
};

#define PpuMapper1 NesPpuMemoryMapper

class NesMapper1Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER1_H
