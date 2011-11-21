#ifndef PATHMANAGER_H
#define PATHMANAGER_H

#include <QStringList>
class QDir;

class PathManager {
public:
	static PathManager *instance();

	QString installationDirPath() const;
	QString userDataDirPath() const;
	QString diskDirPath(const QString &machine) const;
	QString screenShotPath(const QString &machine,
						   const QString &title) const;
	QString stateDirPath(const QString &machine,
						 const QString &title) const;
	QString cheatPath(const QString &machine,
					  const QString &title) const;
	QString homeScreenIconPath(const QString &machine,
							   const QString &title) const;
	QString desktopFilePath(const QString &machine,
							const QString &title) const;

	void buildLocalDirTree();

	void setMachine(const QString &name);

	QString diskDirPath() const;
	QString screenShotPath(const QString &title) const;
	QString stateDirPath(const QString &title) const;
	QString cheatPath(const QString &title) const;

	QStringList machines() const;
private:
	Q_DISABLE_COPY(PathManager)
	PathManager();
	void createMachineSubtree(QDir &dir);

	QString m_machine;
	QStringList m_machines;
	QString m_installationDirPath;
	QString m_userDataDirPath;
	QString m_diskDirBase;

	static PathManager *inst;
};

inline QStringList PathManager::machines() const
{ return m_machines; }

inline QString PathManager::installationDirPath() const
{ return m_installationDirPath; }
inline QString PathManager::userDataDirPath() const
{ return m_userDataDirPath; }

inline PathManager *PathManager::instance() {
	if (!inst)
		inst = new PathManager();
	return inst;
}

#endif // PATHMANAGER_H
