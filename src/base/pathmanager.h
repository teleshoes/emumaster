#ifndef PATHMANAGER_H
#define PATHMANAGER_H

#include "base_global.h"
#include <QStringList>
class QDir;

class BASE_EXPORT PathManager
{
public:
	static PathManager *instance();

	QString installationDirPath() const;
	QString userDataDirPath() const;
	QString diskDirPath(const QString &emu) const;
	QString screenShotPath(const QString &emu,
						   const QString &title) const;
	QString stateDirPath(const QString &emu,
						 const QString &title) const;
	QString homeScreenIconPath(const QString &emu,
							   const QString &title) const;
	QString desktopFilePath(const QString &emu,
							const QString &title) const;

	void buildLocalDirTree();

	void setCurrentEmu(const QString &name);

	QString diskDirPath() const;
	QString screenShotPath(const QString &title) const;
	QString stateDirPath(const QString &title) const;

	QStringList emus() const;
private:
	Q_DISABLE_COPY(PathManager)
	PathManager();
	void createEmusSubtree(QDir &dir);

	QString m_currentEmu;
	QStringList m_emus;
	QString m_installationDirPath;
	QString m_userDataDirPath;
	QString m_diskDirBase;

	static PathManager *inst;
};

inline QStringList PathManager::emus() const
{ return m_emus; }

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
