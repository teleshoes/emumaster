#ifndef MAPPER5_H
#define MAPPER5_H

#include "nesmapper.h"
#include "nescpumemorymapper.h"
#include "nesppumemorymapper.h"

class NesMapper5Data;
class PpuMapper5;

class CpuMapper5 : public NesCpuMemoryMapper {
	Q_OBJECT
public:
	explicit CpuMapper5(NesMapper *mapper);
	~CpuMapper5();
	void reset();
	quint8 read(quint16 address);
	void write(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);

	void save(QDataStream &s);
	bool load(QDataStream &s);

	void updateBanks();
private:
	NesMapper5Data *d;
	PpuMapper5 *m_ppuMapper;
};

class PpuMapper5: public NesPpuMemoryMapper {
	Q_OBJECT
public:
	explicit PpuMapper5(NesMapper *mapper);
	void reset();
	void horizontalSync(int scanline);
	void updateBanks();
private:
	NesMapper5Data *d;
};

class NesMapper5Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER5_H
