#ifndef MAPPER10_H
#define MAPPER10_H

#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"

class PpuMapper10;
class NesMapper10Data;

class CpuMapper10 : public NesCpuMapper {
	Q_OBJECT
public:
	explicit CpuMapper10(NesMapper *mapper);
	~CpuMapper10();
	void reset();
	void writeHigh(quint16 address, quint8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	NesMapper10Data *d;
	PpuMapper10 *ppuMapper;
	friend class PpuMapper10;
};

class PpuMapper10 : public NesPpuMapper {
	Q_OBJECT
public:
	explicit PpuMapper10(NesMapper *mapper);
	void reset();

	void characterLatch(quint16 address);
private:
	NesMapper10Data *d;
};

class NesMapper10Plugin : public NesMapperPlugin {
	Q_OBJECT
public:
	NesMapper *create(NesMachine *machine);
};

#endif // MAPPER10_H
