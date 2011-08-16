#ifndef MAPPER12_H
#define MAPPER12_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class NesMapper12Data;
class PpuMapper12;

class CpuMapper12 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper12(NesMapper *mapper);
	~CpuMapper12();
	void reset();

	void writeLow(quint16 address, quint8 data);
	quint8 readLow(quint16 address);
	void writeHigh(quint16 address, quint8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);

	void updateBanks();
private:
	NesMapper12Data *d;
	PpuMapper12 *ppuMapper;
	friend class PpuMapper12;
};

class PpuMapper12 : public NesPpuMapper {
	Q_OBJECT
public:
	explicit PpuMapper12(NesMapper *mapper);
	void reset();

	void horizontalSync(int scanline);

	void updateBanks();
private:
	NesMapper12Data *d;
	CpuMapper12 *cpuMapper;
	NesPpuRegisters *ppuRegisters;
};

class NesMapper12Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER12_H
