/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef GAMEGENIECODELISTMODEL_H
#define GAMEGENIECODELISTMODEL_H

class GameGenieCode;
class NesMachine;
#include <QAbstractListModel>
#include <QStringList>
#include <QFile>

class GameGenieCodeListModel : public QAbstractListModel {
    Q_OBJECT
public:
	enum RoleType {
		CodeRole = Qt::UserRole+1,
		DescriptionRole,
		EnableRole
	};
	explicit GameGenieCodeListModel(QObject *parent = 0);
	~GameGenieCodeListModel();
	int rowCount(const QModelIndex &parent) const;
	QVariant data(const QModelIndex &index, int role) const;

	void save();
	void load();
	QList<GameGenieCode> enabledList() const;
	Q_INVOKABLE void setEnabled(int i, bool on);
	Q_INVOKABLE void addNew(const QString &code, const QString &description);
	Q_INVOKABLE void removeAt(int i);

	Q_INVOKABLE bool isCodeValid(const QString &s);
private:
	QStringList m_codes;
	QStringList m_descriptions;
	QList<bool> m_enable;
	QFile m_file;
};

#endif // GAMEGENIECODELISTMODEL_H
