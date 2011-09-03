#ifndef MACHINESTATELISTMODEL_H
#define MACHINESTATELISTMODEL_H

class IMachine;
#include <QAbstractListModel>
#include <QStringList>
#include <QFileInfo>
#include <QDir>

class MachineStateListModel : public QAbstractListModel {
	Q_OBJECT
	Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
	enum RoleType {
		NameRole = Qt::UserRole+1,
		ScreenShotUpdate,
		DateTimeRole
	};
	explicit MachineStateListModel(IMachine *machine, const QString &diskName);
	int rowCount(const QModelIndex &parent) const;
	int count() const;
	QVariant data(const QModelIndex &index, int role) const;
	Q_INVOKABLE QString get(int i) const;
	QImage screenShot(int i) const;

	Q_INVOKABLE bool saveState(int i);
	Q_INVOKABLE bool loadState(int i);
	Q_INVOKABLE void removeState(int i);
signals:
	void countChanged();
private:
	int indexOf(int i) const;

	IMachine *m_machine;
	QDir m_dir;
	QFileInfoList m_list;
	int m_maxSaveIndex;
	int m_screenShotUpdateCounter;
};

#endif // MACHINESTATELISTMODEL_H
