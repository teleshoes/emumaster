#ifndef ROMLISTMODEL_H
#define ROMLISTMODEL_H

#include <QAbstractListModel>
#include <QStringList>

class RomListModel : public QAbstractListModel {
    Q_OBJECT
	Q_PROPERTY(QString machineName READ machineName WRITE setMachineName NOTIFY machineNameChanged)
	Q_PROPERTY(int count READ count NOTIFY machineNameChanged)
public:
	enum RoleType {
		NameRole = Qt::UserRole+1,
		AlphabetRole,
		ScreenShotUpdate
	};
    explicit RomListModel(QObject *parent = 0);
	QString machineName() const;
	void setMachineName(const QString &name);
	int rowCount(const QModelIndex &parent) const;
	int count() const;
	QVariant data(const QModelIndex &index, int role) const;
	void updateScreenShot(const QString &name);
	Q_INVOKABLE QString get(int i) const;
	Q_INVOKABLE QString getAlphabet(int i) const;
	Q_INVOKABLE int getScreenShotUpdate(int i) const;
signals:
	void machineNameChanged();
private:
	int m_screenShotUpdateCounter;
	QStringList m_list;
	QString m_machineName;
};

inline QString RomListModel::machineName() const
{ return m_machineName; }

#endif // ROMLISTMODEL_H
