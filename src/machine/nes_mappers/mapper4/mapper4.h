#ifndef MAPPER4_H
#define MAPPER4_H

#include "nesmapper.h"
#include "nescpumemorymapper.h"
#include "nesppumemorymapper.h"

class NesMapper4Data;
class PpuMapper4;

class CpuMapper4 : public NesCpuMemoryMapper {
	Q_OBJECT
public:
	explicit CpuMapper4(NesMapper *mapper);
	~CpuMapper4();
	void reset();
	quint8 read(quint16 address);
	void write(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);

	void save(QDataStream &s);
	bool load(QDataStream &s);

	void updateBanks();
private:
	NesMapper4Data *d;
	PpuMapper4 *m_ppuMapper;
	friend class PpuMapper4;
};

class PpuMapper4: public NesPpuMemoryMapper {
	Q_OBJECT
public:
	explicit PpuMapper4(NesMapper *mapper);
	void reset();
	void horizontalSync(int scanline);
	void updateBanks();
private:
	NesMapper4Data *d;
};

class NesMapper4Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER4_H
