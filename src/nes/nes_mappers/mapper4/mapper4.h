#ifndef MAPPER4_H
#define MAPPER4_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class NesMapper4Data;
class PpuMapper4;

class CpuMapper4 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper4(NesMapper *mapper);
	~CpuMapper4();
	void reset();

	quint8 readLow(quint16 address);
	void writeLow(quint16 address, quint8 data);

	void writeHigh(quint16 address, quint8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);

	void updateBanks();
private:
	NesMapper4Data *d;
	PpuMapper4 *ppuMapper;
	friend class PpuMapper4;
};

class PpuMapper4: public NesPpuMapper {
	Q_OBJECT
public:
	explicit PpuMapper4(NesMapper *mapper);
	void reset();

	void horizontalSync(int scanline);
	void updateBanks();
private:
	NesMapper4Data *d;
	CpuMapper4 *cpuMapper;
	NesPpuRegisters *ppuRegisters;
};

class NesMapper4Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER4_H
