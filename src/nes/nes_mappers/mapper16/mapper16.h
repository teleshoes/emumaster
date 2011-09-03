#ifndef MAPPER16_H
#define MAPPER16_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class NesMapper16Data;
class PpuMapper16;

class CpuMapper16 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper16(NesMapper *mapper);
	~CpuMapper16();

	void reset();

	quint8 readLow(quint16 address);
	void writeLow(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);

	void clock(uint cycles);

	void writeSubA(quint16 address, quint8 data);
	void writeSubB(quint16 address, quint8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	NesMapper16Data *d;
	PpuMapper16 *ppuMapper;
	friend class PpuMapper16;
};

class PpuMapper16 : public NesPpuMapper {
	Q_OBJECT
public:
	explicit PpuMapper16(NesMapper *mapper);
	void reset();

	void horizontalSync(int scanline);
private:
	NesMapper16Data *d;
	CpuMapper16 *cpuMapper;
};

class NesMapper16Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER16_H
