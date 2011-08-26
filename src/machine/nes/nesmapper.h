#ifndef MAPPER_H
#define MAPPER_H

class NesPpuMapper;
class NesCpuMapper;
#include "nes_global.h"
#include "nesmachine.h"
#include <QObject>
#include <QtPlugin>

class NES_EXPORT NesMapper : public QObject {
	Q_OBJECT
	Q_PROPERTY(QString name READ name CONSTANT)
public:
	static NesMapper *load(NesMachine *machine, quint8 type);

	explicit NesMapper(const QString &name, NesMachine *machine);
	void reset();
	NesMachine *machine() const;
	QString name() const;
	NesCpuMapper *cpuMapper() const;
	NesPpuMapper *ppuMapper() const;
	void setMappers(NesCpuMapper *cpuMapper, NesPpuMapper *ppuMapper);

	void saveState(QDataStream &s);
	bool loadState(QDataStream &s);
private:
	NesCpuMapper *m_cpuMapper;
	NesPpuMapper *m_ppuMapper;
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
inline NesCpuMapper *NesMapper::cpuMapper() const
{ return m_cpuMapper; }
inline NesPpuMapper *NesMapper::ppuMapper() const
{ return m_ppuMapper; }

#define NES_MAPPER_PLUGIN_EXPORT(n,name) \
	NesMapper *NesMapper##n##Plugin::create(NesMachine *machine) { \
		NesMapper *mapper = new NesMapper(name, machine); \
		mapper->setMappers(new CpuMapper##n(mapper), new PpuMapper##n(mapper)); \
		mapper->reset(); \
		return mapper; \
	} \
	Q_EXPORT_PLUGIN2(mapper##n, NesMapper##n##Plugin)

#endif // MAPPER_H
