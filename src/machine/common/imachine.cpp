#include "imachine.h"
#include <QPluginLoader>

IMachine::IMachine(const QString &name, QObject *parent) :
	QObject(parent),
	m_frameRate(1),
	m_audioEnable(true),
	m_audioStereoEnable(true),
	m_audioSampleRate(22050),
	m_name(name) {
}

IMachine::~IMachine() {
}

void IMachine::setFrameRate(qreal rate)
{ m_frameRate = rate; }

void IMachine::setPadKey(PadKey key, bool state) {
	Q_UNUSED(key)
	Q_UNUSED(state)
}

IMachine *IMachine::loadMachine(const QString &name) {
#if defined(Q_OS_WIN)
	QString path = QString("../lib/%1.dll").arg(name);
#else
	QString path = QString("../lib/lib%1.so").arg(name);
#endif
	QPluginLoader loader(path);
	return qobject_cast<IMachine *>(loader.instance());
}

void IMachine::setGameGenieCodeList(const QList<GameGenieCode> &codes) {
	Q_UNUSED(codes)
}
