#include "nesmapper.h"
#include <QPluginLoader>

NesMapper::NesMapper(const QString &name, NesMachine *machine) :
	QObject(machine),
	m_name(name),
	m_cpuMappper(0),
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
	m_cpuMappper = cpuMapper;
	m_ppuMapper = ppuMapper;
}
