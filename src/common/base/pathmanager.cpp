#include "pathmanager.h"
#include <QDir>
#include <stdlib.h>

PathManager *PathManager::inst = 0;

PathManager::PathManager() {
	m_machines << "nes";
	m_machines << "gba";
	m_machines << "snes";
	m_machines << "psx";
	m_machines << "amiga";

	m_installationDirPath = "/opt/emumaster";
	m_userDataDirPath = QString("%1/.emumaster").arg(getenv("HOME"));
	m_diskDirBase = QString("%1/MyDocs/emumaster").arg(getenv("HOME"));
}

void PathManager::createMachineSubtree(QDir &dir) {
	for (int i = 0; i < m_machines.size(); i++)
		dir.mkdir(m_machines.at(i));
}

void PathManager::buildLocalDirTree() {
	QDir dir(getenv("HOME"));
	dir.mkdir(".emumaster");
	dir.cd(".emumaster");
	dir.mkdir("state");
	dir.mkdir("icon");
	dir.mkdir("screenshot");
	dir.mkdir("cheat");

	dir.cd("state");		createMachineSubtree(dir); dir.cdUp();
	dir.cd("screenshot");	createMachineSubtree(dir); dir.cdUp();
	dir.cd("cheat");		createMachineSubtree(dir); dir.cdUp();

	dir = QDir(getenv("HOME"));
	dir.cd("MyDocs");
	dir.mkdir("emumaster");
	dir.cd("emumaster");
	dir.mkdir("covers");
	createMachineSubtree(dir);
}

QString PathManager::diskDirPath(const QString &machine) const {
	return QString("%1/%2").arg(m_diskDirBase).arg(machine);
}

QString PathManager::diskDirPath() const {
	return diskDirPath(m_machine);
}

QString PathManager::screenShotPath(const QString &machine,
									const QString &title) const {
	return QString("%1/screenshot/%2/%3.jpg")
			.arg(userDataDirPath())
			.arg(machine)
			.arg(title);
}

QString PathManager::screenShotPath(const QString &title) const {
	return screenShotPath(m_machine, title);
}

void PathManager::setMachine(const QString &name) {
	Q_ASSERT(m_machines.contains(name));
	m_machine = name;
}

QString PathManager::stateDirPath(const QString &machine,
								  const QString &title) const {
	return QString("%1/state/%2_%3.png")
			.arg(userDataDirPath())
			.arg(machine)
			.arg(title);
}

QString PathManager::stateDirPath(const QString &title) const {
	return stateDirPath(m_machine, title);
}

QString PathManager::homeScreenIconPath(const QString &machine,
										const QString &title) const {
	return QString("%1/icon/%2_%3.png")
			.arg(userDataDirPath())
			.arg(machine)
			.arg(title);
}

QString PathManager::desktopFilePath(const QString &machine,
									 const QString &title) const {
#if defined(MEEGO_EDITION_HARMATTAN)
	return QString("%1/.local/share/applications/emumaster_%2_%3.desktop")
#elif defined(Q_WS_MAEMO_5)
	return QString("%1/.local/share/applications/hildon/emumaster_%2_%3.desktop")
#endif
			.arg(getenv("HOME"))
			.arg(machine)
			.arg(title);
}

QString PathManager::cheatPath(const QString &machine,
							   const QString &title) const {
	return QString("%1/cheat/%2/%3.jpg")
			.arg(userDataDirPath())
			.arg(machine)
			.arg(title);
}

QString PathManager::cheatPath(const QString &title) const {
	return cheatPath(m_machine, title);
}
