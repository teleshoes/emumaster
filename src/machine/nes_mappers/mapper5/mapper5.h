#ifndef MAPPER5_H
#define MAPPER5_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class NesMapper5Data;
class PpuMapper5;

class CpuMapper5 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper5(NesMapper *mapper);
	~CpuMapper5();
	void reset();

	quint8 readLow(quint16 address);
	void writeLow(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);

	void setBankSram(quint8 page, quint8 data);
	void setBank(quint16 address, quint8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	NesMapper5Data *d;
	PpuMapper5 *ppuMapper;

	friend class PpuMapper5;
};

class PpuMapper5: public NesPpuMapper {
	Q_OBJECT
public:
	explicit PpuMapper5(NesMapper *mapper);
	void reset();

	void updateBanks();

	void horizontalSync(int scanline);
	void extensionLatchX(uint x);
	void extensionLatch(quint16 address, quint8 *plane1, quint8 *plane2, quint8 *attribute);
private:
	NesMapper5Data *d;
	CpuMapper5 *cpuMapper;
	NesPpu *ppu;
	NesPpuRegisters *ppuRegisters;

	friend class CpuMapper5;
};

class NesMapper5Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER5_H
