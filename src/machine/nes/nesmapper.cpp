#include "nesmapper.h"
#include "nescpumemorymapper.h"
#include "nesppumemorymapper.h"
#include <QPluginLoader>
#include <QDataStream>

NesMapper::NesMapper(const QString &name, NesMachine *machine) :
	QObject(machine),
	m_name(name),
	m_cpuMapper(0),
	m_ppuMapper(0) {
}

NesMapper *NesMapper::load(NesMachine *machine, quint8 type) {
#if defined(Q_OS_WIN)
	QString path = QString("nes_mappers/mapper%1.dll").arg(type);
#else
	QString path = QString("nes_mappers/libmapper%1.so").arg(type);
#endif
	QPluginLoader loader(path);
	NesMapperPlugin *plugin = qobject_cast<NesMapperPlugin *>(loader.instance());
	if (!plugin)
		return 0;
	return plugin->create(machine);
}

void NesMapper::setMappers(NesCpuMemoryMapper *cpuMapper, NesPpuMemoryMapper *ppuMapper) {
	m_cpuMapper = cpuMapper;
	m_ppuMapper = ppuMapper;
}

void NesMapper::reset() {
	m_irqOut = false;
	emit irq_o(false);
	if (m_cpuMapper)
		m_cpuMapper->reset();
	if (m_ppuMapper)
		m_ppuMapper->reset();
}

void NesMapper::saveState(QDataStream &s) {
	m_cpuMapper->save(s);
	m_ppuMapper->save(s);
	s << m_irqOut;
}

bool NesMapper::loadState(QDataStream &s) {
	if (!m_cpuMapper->load(s))
		return false;
	if (!m_ppuMapper->load(s))
		return false;
	s >> m_irqOut;
	return true;
}
