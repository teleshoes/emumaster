#ifndef GAMEGENIECODELISTMODEL_H
#define GAMEGENIECODELISTMODEL_H

class MachineView;
class GameGenieCode;
#include <QAbstractListModel>
#include <QStringList>

class GameGenieCodeListModel : public QAbstractListModel {
    Q_OBJECT
public:
	enum RoleType {
		CodeRole = Qt::UserRole+1,
		DescriptionRole,
		EnableRole
	};
	explicit GameGenieCodeListModel(MachineView *machineView);
	~GameGenieCodeListModel();
	int rowCount(const QModelIndex &parent) const;
	QVariant data(const QModelIndex &index, int role) const;

	void save();
	void load();
	QList<GameGenieCode> enabledList() const;
	Q_INVOKABLE void setEnabled(int i, bool on);
	Q_INVOKABLE void addNew(const QString &code, const QString &description);
	Q_INVOKABLE void removeAt(int i);
private:
	QString filePath();

	QStringList m_codes;
	QStringList m_descriptions;
	QList<bool> m_enable;
};

#endif // GAMEGENIECODELISTMODEL_H