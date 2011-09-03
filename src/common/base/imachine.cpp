#include "imachine.h"
#include <QPluginLoader>
#include <QSettings>
#include <QDir>
#include <stdlib.h>

IMachine::IMachine(const QString &name, QObject *parent) :
	QObject(parent),
	m_frameRate(1),
	m_name(name) {
}

IMachine::~IMachine() {
}

void IMachine::setFrameRate(qreal rate)
{ m_frameRate = rate; }
void IMachine::setVideoSrcRect(const QRectF &rect)
{ m_videoSrcRect = rect; }

void IMachine::saveSettings(QSettings &s)
{ Q_UNUSED(s) }
void IMachine::loadSettings(QSettings &s)
{ Q_UNUSED(s) }

QString IMachine::installationDirPath()
{ return QString("/opt/emumaster"); }
QString IMachine::userDataDirPath()
{ return QString("%1/.emumaster").arg(getenv("HOME")); }
QString IMachine::diskDirPath(const QString &machineName)
{ return QString("%1/MyDocs/emumaster/%2").arg(getenv("HOME")).arg(machineName); }

void IMachine::buildLocalDirTree() {
	QDir dir(getenv("HOME"));
	dir.mkdir(".emumaster");
	dir.cd(".emumaster");
	dir.mkdir("state");
	dir.mkdir("icon");
	dir.mkdir("screenshot");
	dir.mkdir("cheat");

	dir = QDir(getenv("HOME"));
	dir.cd("MyDocs");
	dir.mkdir("emumaster");
	dir.cd("emumaster");
	dir.mkdir("nes");
	dir.mkdir("gba");
	// TODO add directory for other consoles
}

QString IMachine::screenShotPath(const QString &diskName) const {
	return QString("%1/screenshot/%2_%3.jpg")
			.arg(userDataDirPath())
			.arg(m_name)
			.arg(diskName);
}
