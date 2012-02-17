#include "pathmanager.h"
#include <QDir>
#include <stdlib.h>

PathManager *PathManager::inst = 0;

PathManager::PathManager() {
	m_emus << "nes";
	m_emus << "gba";
	m_emus << "snes";
	m_emus << "psx";
	m_emus << "amiga";
	m_emus << "pico";

	m_installationDirPath = "/opt/emumaster";
	m_userDataDirPath = QString("%1/.emumaster").arg(getenv("HOME"));
	m_diskDirBase = QString("%1/MyDocs/emumaster").arg(getenv("HOME"));
}

void PathManager::createEmusSubtree(QDir &dir) {
	for (int i = 0; i < m_emus.size(); i++)
		dir.mkdir(m_emus.at(i));
}

void PathManager::buildLocalDirTree() {
	QDir dir(getenv("HOME"));
	dir.mkdir(".emumaster");
	dir.cd(".emumaster");
	dir.mkdir("state");
	dir.mkdir("icon");
	dir.mkdir("screenshot");

	dir.cd("state");		createEmusSubtree(dir); dir.cdUp();
	dir.cd("screenshot");	createEmusSubtree(dir); dir.cdUp();

	dir = QDir(getenv("HOME"));
	dir.cd("MyDocs");
	dir.mkdir("emumaster");
	dir.cd("emumaster");
	dir.mkdir("covers");
	createEmusSubtree(dir);
}

QString PathManager::diskDirPath(const QString &emu) const {
	return QString("%1/%2").arg(m_diskDirBase).arg(emu);
}

QString PathManager::diskDirPath() const {
	return diskDirPath(m_currentEmu);
}

QString PathManager::screenShotPath(const QString &emu,
									const QString &title) const {
	return QString("%1/screenshot/%2/%3.jpg")
			.arg(userDataDirPath())
			.arg(emu)
			.arg(title);
}

QString PathManager::screenShotPath(const QString &title) const {
	return screenShotPath(m_currentEmu, title);
}

void PathManager::setCurrentEmu(const QString &name) {
	Q_ASSERT(m_emus.contains(name));
	m_currentEmu = name;
}

QString PathManager::stateDirPath(const QString &emu,
								  const QString &title) const {
	return QString("%1/state/%2/%3")
			.arg(userDataDirPath())
			.arg(emu)
			.arg(title);
}

QString PathManager::stateDirPath(const QString &title) const {
	return stateDirPath(m_currentEmu, title);
}

QString PathManager::homeScreenIconPath(const QString &emu,
										const QString &title) const {
	return QString("%1/icon/%2_%3.png")
			.arg(userDataDirPath())
			.arg(emu)
			.arg(title);
}

QString PathManager::desktopFilePath(const QString &emu,
									 const QString &title) const {
#if defined(MEEGO_EDITION_HARMATTAN)
	return QString("%1/.local/share/applications/emumaster_%2_%3.desktop")
			.arg(getenv("HOME"))
			.arg(emu)
			.arg(title);
#elif defined(Q_WS_MAEMO_5)
	return QString("%1/.local/share/applications/hildon/emumaster_%2_%3.desktop")
			.arg(getenv("HOME"))
			.arg(emu)
			.arg(title);
#endif
}
