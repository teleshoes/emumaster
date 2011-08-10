#ifndef MAPPER_H
#define MAPPER_H

class NesPpuMemoryMapper;
class NesCpuMemoryMapper;
#include "nes_global.h"
#include "nesmachine.h"
#include <QObject>
#include <QtPlugin>

class NES_EXPORT NesMapper : public QObject {
	Q_OBJECT
public:
	static NesMapper *load(NesMachine *machine, quint8 type);

	explicit NesMapper(const QString &name, NesMachine *machine);
	NesMachine *machine() const;
	QString name() const;
	NesCpuMemoryMapper *cpuMemory() const;
	NesPpuMemoryMapper *ppuMemory() const;
	void setMappers(NesCpuMemoryMapper *cpuMapper, NesPpuMemoryMapper *ppuMapper);
private:
	NesCpuMemoryMapper *m_cpuMappper;
	NesPpuMemoryMapper *m_ppuMapper;
	QString m_name;
};

class NES_EXPORT NesMapperPlugin : public QObject {
	Q_OBJECT
public:
	virtual NesMapper *create(NesMachine *machine) = 0;
};

inline NesMachine *NesMapper::machine() const
{ return static_cast<NesMachine *>(parent()); }
inline QString NesMapper::name() const
{ return m_name; }
inline NesCpuMemoryMapper *NesMapper::cpuMemory() const
{ return m_cpuMappper; }
inline NesPpuMemoryMapper *NesMapper::ppuMemory() const
{ return m_ppuMapper; }

#define NES_MAPPER_PLUGIN_SOURCE(n,name) \
	NesMapper *NesMapper##n##Plugin::create(NesMachine *machine) { \
		NesMapper *mapper = new NesMapper(name, machine); \
		mapper->setMappers(new CpuMapper##n(mapper), new PpuMapper##n(mapper)); \
		return mapper; \
	} \
	Q_EXPORT_PLUGIN2(mapper##n, NesMapper##n##Plugin)

#endif // MAPPER_H
