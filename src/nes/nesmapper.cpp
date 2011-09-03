#include "nesmapper.h"
#include "nescpumapper.h"
#include "nesppumapper.h"
#include <QPluginLoader>
#include <QDataStream>

NesMapper::NesMapper(const QString &name, NesMachine *machine) :
	QObject(machine),
	m_cpuMapper(0),
	m_ppuMapper(0),
	m_name(name) {
}

#include <QFileInfo>

NesMapper *NesMapper::load(NesMachine *machine, quint8 type) {
#if defined(Q_OS_WIN)
	QString path = QString("../lib/nes_mapper/mapper%1.dll").arg(type);
#else
	QString path = QString("../lib/nes_mapper/libmapper%1.so").arg(type);
#endif
	QPluginLoader loader(path);
	NesMapperPlugin *plugin = qobject_cast<NesMapperPlugin *>(loader.instance());
	if (!plugin) {
		qDebug("%s", qPrintable(loader.errorString()));
		return 0;
	}
	return plugin->create(machine);
}

void NesMapper::setMappers(NesCpuMapper *cpuMapper, NesPpuMapper *ppuMapper) {
	m_cpuMapper = cpuMapper;
	m_ppuMapper = ppuMapper;
}

void NesMapper::reset() {
	Q_ASSERT(m_cpuMapper != 0 && m_ppuMapper != 0);
	m_cpuMapper->reset();
	m_cpuMapper->processGameGenieCodes();
	m_ppuMapper->reset();
}
