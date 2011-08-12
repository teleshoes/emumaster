#ifndef MAPPER51_H
#define MAPPER51_H

#include "nesmapper.h"
#include "nescpumemorymapper.h"
#include "nesppumemorymapper.h"

class PpuMapper51;

class CpuMapper51 : public NesCpuMemoryMapper {
	Q_OBJECT
public:
	explicit CpuMapper51(NesMapper *mapper);
	void reset();
	quint8 read(quint16 address);
	void write(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);

	void save(QDataStream &s);
	bool load(QDataStream &s);
private:
	void updateBanks();

	quint32 mode, bank;
	quint8 *m_lowMemBankData;
};

#define PpuMapper51 NesPpuMemoryMapper

class NesMapper51Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER51_H
