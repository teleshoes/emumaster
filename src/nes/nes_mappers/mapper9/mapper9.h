#ifndef MAPPER9_H
#define MAPPER9_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class PpuMapper9;
class NesMapper9Data;

class CpuMapper9 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper9(NesMapper *mapper);
	~CpuMapper9();
	void reset();
	void writeHigh(quint16 address, quint8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	NesMapper9Data *d;
	PpuMapper9 *ppuMapper;
	friend class PpuMapper9;
};

class PpuMapper9 : public NesPpuMapper {
	Q_OBJECT
public:
	explicit PpuMapper9(NesMapper *mapper);
	void reset();

	void characterLatch(quint16 address);
private:
	NesMapper9Data *d;
};

class NesMapper9Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER9_H
